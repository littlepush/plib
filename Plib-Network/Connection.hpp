/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: Connection.hpp
* Propose  			: Connection Maintainer Framework. 
* 
* Current Version	: 1.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2013-02-17
*/

#pragma once

#ifndef _PLIB_NETWORK_CONNECTION_HPP_
#define _PLIB_NETWORK_CONNECTION_HPP_

#include <Plib-Network/BasicNetwork.hpp>
#include <Plib-Network/PeerInfo.hpp>
#include <Plib-Network/Socket.hpp>
#include <Plib-Network/Request.hpp>
#include <Plib-Network/Response.hpp>

#include <Plib-Threading/Thread.hpp>

namespace Plib
{
	namespace Network
	{

		#define __REF			Plib::Generic::Reference
		#define __DELG			Plib::Generic::Delegate
		using Plib::Text::String;

		// Default delegate object
		template < typename _TyReq >
		class DefaultSuccessCallBack
		{
		public:
			void operator() (__REF<_TyReq> request, __REF<typename _TyReq::TResp> response) const
			{
				PINFO( "[Request: " << request->Identify << 
					"] Successed." );
			}
		};

		template < typename _TyReq >
		class DefaultFailedCallBack
		{
		public:
			void operator() ( __REF<_TyReq> request, String errorInfo ) const
			{
				PERROR( "[Request: " << request->Identify << 
					"] Failed, Error Info: <" << errorInfo << ">." );
			}
		};

		template < typename _TyReq >
		struct RequestContext
		{
			typedef typename _TyReq::TResp 					_TyResp;
			typedef __REF<_TyReq>							_tRequest;
			typedef __REF<_TyResp>							_tResponse;
			typedef __DELG<void(_tRequest, _tResponse)>		_tSuccess;
			typedef __DELG<void(_tRequest, String)>			_tFailed;

			_tRequest				request;
			_tSuccess 				success;
			_tFailed 				failed;

			// Constructure
			RequestContext<_TyReq>( ){ }
			RequestContext<_TyReq>( _tRequest r, _tSuccess s, _tFailed f ) 
				: request( r ), success( s ), failed( f ) { }
			RequestContext<_TyReq>( const RequestContext<_TyReq> & rhs )
				: request( rhs.request ), success( rhs.success ), failed( rhs.failed )
			{
			}
		};

		// Object Cache
		template < typename _TyReq >
		class ConnectionRequestCache
		{
		protected:

			typedef std::map< String, RequestContext< _TyReq > >	TCache;
			typedef typename TCache::iterator						ICache;

			// Cache Core
			std::map< String, RequestContext< _TyReq > >			mCache;

			//static PLIB_THREAD_SAFE_DEFINE;
			static Plib::SpinLocker 								_locker;

			// For Singleton usage constructure
			ConnectionRequestCache( ) { }

			static ConnectionRequestCache<_TyReq> & self( )
			{
				static ConnectionRequestCache _self;
				return _self;
			}
		public:

			static void AddRequestContext( RequestContext<_TyReq> & context )
			{
				SPINLOCK(_locker);
				// set value
				self().mCache[context.request->Identify] = context;
			}

			static Request * GetRequestByIdentify( String identify )
			{
				SPINLOCK(_locker);
				ICache _context = self().mCache.find( identify );
				if ( _context == self().mCache.end() ) return NULL;
				return (Request *)&(*(_context->second.request));
			}

			static void RemoveRequestByIdentify( String identify )
			{
				SPINLOCK(_locker);
				ICache _context = self().mCache.find( identify );
				if ( _context == self().mCache.end() ) return;
				self().mCache.erase( _context );
			}
		};

		template < typename _TyReq >
		SpinLocker ConnectionRequestCache< _TyReq >::_locker;

		//class TcpConnectCreator

		template < 
			typename _TyConnect = TcpSocketConnect, 		// Function object to connect to peer
			typename _TyWrite = TcpSocketWrite, 			// Function object to write data
			typename _TyRead = TcpSocketRead 				// Function object to read data
		>
		class Connection
		{
			typedef Socket< _TyConnect, _TyWrite, _TyRead >		Connector;
			typedef __DELG< Request *(String) >					dGetRequest;
			typedef std::pair< String, dGetRequest >			tPendingReq;
			typedef Plib::Threading::Thread< void () >			tWorking;
			typedef Plib::Generic::Queue< __REF<tWorking> >		tThreadQueue;

		protected:
			Plib::Generic::Queue< tPendingReq >					m_reqQueue;
			Plib::Threading::Mutex								m_queueLocker;
			Plib::Threading::Thread< void () >					m_waitThread;

		protected:
			tThreadQueue										m_workingQueue;
			Plib::Threading::Semaphore 							m_workingSem;
			Uint32												m_workingCount;
			Uint32												m_workingMax;

		protected:
			__REF<tWorking> __threadCreater
			{
				__REF<tWorking> _rt;
				//_rt->SetStackSize( 8M );
				_rt->Jobs += std::make_pair( this, 
					&Connection<_TyConnect, _TyWrite, _TyRead>::__thread_WorkingOnRequest);
				return _rt;
			}
			// Working Threading
			void __thread_WorkingOnRequest( )
			{

			}
			// Global Threading
			void __thread_WaitForNewRequest( )
			{
				//PTRACE("Thread Started");
				while ( Plib::Threading::ThreadSys::WaitForSignal() )
				{
					// Check
					if ( !Plib::Threading::ThreadSys::Running() ) break;
/*
					// The the working thread
					while ( !m_workingSem.Get(500) ) {
						if ( !Plib::Threading::ThreadSys::Running() ) return;
					}

					tWorking t = m_workingQueue.Top();
					m_workingQueue.Pop();
*/
					//PTRACE("Get a request.");
					m_queueLocker.Lock();
					tPendingReq _pending = m_reqQueue.Head();
					m_reqQueue.Pop();
					m_queueLocker.UnLock();

					//Request *_req = _get()
					PTRACE( "Get Request: " << _pending.first );
					Request *_req = _pending.second(_pending.first);
					PeerInfo _peerInfo = _req->RequestPeerInfo();
					PTRACE( "Request to " << _peerInfo );

					Connector _conn;
					if ( _conn.Connect(_peerInfo) ){
						_conn.Write(_req->generateFullPackage());
					}
				}
			}

		protected:

			// Singleton Constructure
			Connection< _TyConnect, _TyWrite, _TyRead >( ) 
			{
				m_waitThread.Jobs += std::make_pair( this, 
					&Connection<_TyConnect, _TyWrite, _TyRead>::__thread_WaitForNewRequest );
				m_waitThread.Start();
			}

			~Connection< _TyConnect, _TyWrite, _TyRead >( )
			{
				m_waitThread.Stop( true );
			}

			// Singleton Object
			static Connection & self() {
				static Connection _self;
				return _self;
			}

			template < typename _TyReq >
			void receiveNewRequest( __REF<_TyReq> request, 
				__DELG<void(__REF<_TyReq>, __REF<typename _TyReq::TResp>)> & onSuccess, 
				__DELG<void(__REF<_TyReq>, String)> & onFailed )
			{
				// Generate Context and callback delegate
				RequestContext<_TyReq> context(request, onSuccess, onFailed);
				dGetRequest _get(&ConnectionRequestCache<_TyReq>::GetRequestByIdentify);
				tPendingReq _pending(request->Identify, _get);
				// Add Context to the cache
				ConnectionRequestCache< _TyReq >::AddRequestContext( context );

				// Tell the working thread receive new connection request.

				// RWLock
				Plib::Threading::Locker _lq( m_queueLocker );
				// PushBack(_get)
				this->m_reqQueue.Push( _pending );
				// ReleaseSemaphore
				//this->m_queueSem.Release( );
				this->m_waitThread.GiveSignal();
			}

		public:
			template < typename _TyReq >
			static void Async( __REF<_TyReq> request, 
				__DELG<void(__REF<_TyReq>, __REF<typename _TyReq::TResp>)> onSuccess, 
				__DELG<void(__REF<_TyReq>, String)> onFailed )
			{
				if ( request->Identify.Size() == 0 ) {
					request->SetIdentify( Plib::Utility::Identify::New() );
				}
				self().receiveNewRequest(request, onSuccess, onFailed);
			}

			template < typename _TyReq >
			static void Async( __REF<_TyReq> request, 
				__DELG<void(__REF<_TyReq>, __REF<typename _TyReq::TResp>)> onSuccess)
			{
				if ( request->Identify.Size() == 0 ) {
					request->SetIdentify( Plib::Utility::Identify::New() );
				}
				DefaultFailedCallBack<_TyReq> _defaultFailed;
				__DELG< void(__REF<_TyReq>, String) > _onFailed(_defaultFailed);
				self().receiveNewRequest(request, onSuccess, _onFailed);
			}

			template < typename _TyReq >
			static void Async( __REF<_TyReq> request )
			{
				if ( request->Identify.Size() == 0 ) {
					request->SetIdentify( Plib::Utility::Identify::New() );
				}

				DefaultSuccessCallBack<_TyReq> _defaultSuccess;
				__DELG< void(__REF<_TyReq>, __REF<typename _TyReq::TResp>) > _onSuccess(_defaultSuccess);

				DefaultFailedCallBack<_TyReq> _defaultFailed;
				__DELG< void(__REF<_TyReq>, String) > _onFailed(_defaultFailed);

				self().receiveNewRequest(request, _onSuccess, _onFailed);
			}
		};

		typedef Connection< >		TcpConnection;
	}
}

#endif // plib.network.connection.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */
