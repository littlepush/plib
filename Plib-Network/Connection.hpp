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
		using Plib::Threading::Mutex;
		using Plib::Threading::Semaphore;

		// Typedef
		typedef Plib::Generic::Delegate< void ( IRequest * ) > tReqSuccess;
		typedef Plib::Generic::Delegate< void ( IRequest *, String ) > tReqFailed;

		typedef Plib::Threading::Thread< void () >	tReqWorker;
		typedef Plib::Generic::Reference< tReqWorker > refWorker;

		class RequestSuccessCallback
		{
		public:
			// Just output the log with level info.
			void operator() ( IRequest *req ) const
			{
				PINFO( "[Request: " << req->Identify << "] Successed.");
			}

			operator tReqSuccess ()
			{
				tReqSuccess _delegate( *this );
				return _delegate;
			}
		};

		class RequestFailedCallback
		{
		public:
			// Just output the log with level error.
			void operator() ( IRequest *req, String errorMsg) const
			{
				PERROR( "[Request: " << req->Identify << 
					"] Failed, Error Info: <" << errorMsg << ">." );
			}

			operator tReqFailed ()
			{
				tReqFailed _delegate( *this );
				return _delegate;
			}
		};

		//class TcpConnectCreator
		class Connection
		{
		protected:
			Plib::Generic::Queue< IRequest * >			m_pendingQueue;
			Plib::Generic::Queue< IRequest * >			m_idleQueue;
			Plib::Generic::Queue< IRequest * >			m_fetchingQueue;
			Semaphore									m_pendingSem;
			Semaphore									m_idleSem;
			Semaphore									m_fetchingSem;
			Mutex										m_pendingMutex;
			Mutex 										m_idleMutex;
			Mutex 										m_fetchingMutex;

			Plib::Generic::Array< refWorker >			m_fetchingWorkerList;
			Plib::Generic::Array< refWorker >			m_postingWorkingList;

		protected:
			Uint32										m_maxPostingWorkingCount;
			Uint32										m_maxFetchingWorkingCount;

		protected:
			refWorker __fetcherCreater()
			{
				refWorker _refWorker;
				_refWorker->Jobs += std::make_pair( this, 
					&Connection::__thread_ReceiveResponse);
				// Start the thread automatically.
				_refWorker->Start();
				return _refWorker;
			}
			refWorker __posterCreater()
			{
				refWorker _refWorker;
				_refWorker->Jobs += std::make_pair( this,
					&Connection::__thread_SendRequest);
				// Start the thread automatically.
				_refWorker->Start();
				return _refWorker;
			}

			void __thread_ReceiveResponse( )
			{
				while ( Plib::Threading::ThreadSys::Running() ) {
					// Fetch a sent request from queue
					// Wait for the response data
					// Generate the response object
					// Tell the delegate to call back
					// If the request is set keepAlive
					// move the request to the idle list
				}
			}
			void __thread_SendRequest( )
			{
				while ( Plib::Threading::ThreadSys::Running() ) {
					// Fetch a pending request
					// check if the request's socket has been connected
					// send the package
					// move the request to the fetch list
				}
			}

		protected:

			// Singleton Constructure
			Connection( ):  m_maxPostingWorkingCount(1), m_maxFetchingWorkingCount(1)
			{
				// Create the initialized posting worker
				refWorker _posterWorker = this->__posterCreater();
				m_postingWorkingList.PushBack(_posterWorker);

				// Create the first fetching worker
				refWorker _fetcherWorker = this->__fetcherCreater();
				m_fetchingWorkerList.PushBack(_fetcherWorker);
			}

			~Connection( )
			{
				// Close all worker
				for ( int i = 0; i < m_postingWorkingList.Size(); ++i ) {
					m_postingWorkingList[i]->Stop(true);
				}
				for ( int i = 0; i < m_fetchingWorkerList.Size(); ++i ) {
					m_fetchingWorkerList[i]->Stop(true);
				}
			}

			// Singleton Object
			static Connection & self() {
				static Connection _self;
				return _self;
			}
		public:
			// Async Sending Request
			static void sendAsyncRequest( 
				IRequest * req, 
				tReqSuccess success = RequestSuccessCallback(),
				tReqFailed failed = RequestFailedCallback()
				)
			{

			}
		};
	}
}

#endif // plib.network.connection.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */
