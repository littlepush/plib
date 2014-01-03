/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: Request.hpp
* Propose  			: Basic Request Interface
* 
* Current Version	: 1.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2013-02-17
*/

#pragma once

#ifndef _PLIB_NETWORK_REQUEST_HPP_
#define _PLIB_NETWORK_REQUEST_HPP_

#include <Plib-Network/BasicNetwork.hpp>
#include <Plib-Network/PeerInfo.hpp>
#include <Plib-Network/Response.hpp>

namespace Plib
{
	namespace Network
	{
		using Plib::Text::String;

		// Request Interface, which is an ABC
		class IRequest
		{
		protected:
			PeerInfo					m_peerInfo;
			NData						m_packageBuffer;
			String 						m_identify;

			// Connection Info.
			ISocket						*m_socket;
			IResponse					*m_response;
		public:
			IRequest( )
				: Identify(m_identify)
			{
				// Nothing
			}
			IRequest( const String & host, Uint32 port, Uint32 timeout = 1000 ) 
				: Identify(m_identify)
			{
				m_peerInfo.Address.DeepCopy(host);
				m_peerInfo.Port = port;
				m_peerInfo.ConnectTimeOut = timeout; 
			}
			IRequest( const PeerInfo & peer ) : Identify(m_identify) 
			{ m_peerInfo = peer; }

			virtual ~IRequest( ) {
			};
		public:
			// Before the connection try to send the request, 
			// will invoke this method to connect to the peer.
			virtual bool InitializeConnector() = 0;

			// Before the connection try to receive response data,
			// will invoke this method to initialize the response
			// object.
			virtual bool InitializeResponser() = 0;
		public:

			// Get the request's peer infomation
			const PeerInfo & RequestPeerInfo( ) const { return m_peerInfo; }
			// Get the connection socket
			ISocket * Connector( )const { return m_socket; }
			// Get the responser object
			IResponse * Responser( )const { return m_response; }
		public:
			// Public Properties
			const String &				Identify;
			void SetIdentify( const String & identify ) {
				this->m_identify.DeepCopy( identify );
			}
		public:

			// After setting properties of the specified type request.
			// System will invoke generateFullPackage to get write data.
			virtual const NData & generateFullPackage( ) = 0;
		};
	}
}


#endif // plib.network.request.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */
