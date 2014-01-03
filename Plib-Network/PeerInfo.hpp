/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: PeerInfo.hpp
* Propose  			: Network Peer Infomation Structure
* 
* Current Version	: 1.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2013-02-17
*/

#pragma once

#ifndef _PLIB_NETWORK_PEERINFO_HPP_
#define _PLIB_NETWORK_PEERINFO_HPP_

#include <Plib-Text/Text.hpp>
#include <Plib-Network/BasicNetwork.hpp>

namespace Plib
{
	namespace Network
	{
		using Plib::Text::String;
		class PeerInfo
		{
		public:
			// Basic Peer Info, Address:Port
			String 				Address;
			Uint32				Port;
			// Time out setting
			union {
			// For outgoing request, use connect timeout
			// For incoming request, use read timeout
			// For server response, use send timeout
			Uint32				ConnectTimeOut;
			Uint32				SendTimeOut;
			Uint32				ReadTimeOut;
			};

			// If should the handler maintain the connection to the peer.
			bool 				KeepAlive;

		private:
			void _getPeerInfoFromSocket( SOCKET_T hSo )
			{
				if ( SOCKET_NOT_VALIDATE(hSo) ) return;

				struct sockaddr_in _addr;
				socklen_t _addrLen = sizeof(_addr);
				memset( &_addr, 0, sizeof(_addr) );
				if ( 0 == getpeername( hSo, (struct sockaddr *)&_addr, &_addrLen ) )
				{
					this->Port = ntohs(_addr.sin_port);
					this->Address = String::Parse("%u.%u.%u.%u",
						(unsigned int)(_addr.sin_addr.s_addr >> (0 * 8)) & 0x00FF,
						(unsigned int)(_addr.sin_addr.s_addr >> (1 * 8)) & 0x00FF,
						(unsigned int)(_addr.sin_addr.s_addr >> (2 * 8)) & 0x00FF,
						(unsigned int)(_addr.sin_addr.s_addr >> (3 * 8)) & 0x00FF );
				}
			}
		public:
			PeerInfo( ) : 
				Address("0.0.0.0"), Port(0), ConnectTimeOut(1000), KeepAlive(false) { }
			PeerInfo( SOCKET_T hSo, bool keepAlive = false ) : 
				Address("0.0.0.0"), Port(0), 
				ConnectTimeOut(1000), KeepAlive(keepAlive) 
			{
				// Init the peer info from a connected socket
				this->_getPeerInfoFromSocket( hSo );
			}

			void GetRemotePeerInfo( SOCKET_T hSo ){ this->_getPeerInfoFromSocket( hSo ); }

			PeerInfo & operator = ( const PeerInfo & rhs )
			{
				if ( this == &rhs ) return *this;
				this->Address.DeepCopy( rhs.Address );
				this->Port = rhs.Port;
				this->ConnectTimeOut = rhs.ConnectTimeOut;
				return *this;
			}

			bool operator == ( const PeerInfo & rhs ) const
			{
				if ( this == &rhs ) return true;
				return false;
			}
		};

		// Output
		std::ostream & operator << ( std::ostream & os, const PeerInfo & peerInfo ) {
			os << "<" << peerInfo.Address << ":" << peerInfo.Port << ">";
			return os;
		}
	}
}

#endif // plib.network.peerinfo.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */
