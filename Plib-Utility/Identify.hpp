/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: Identify.hpp
* Propose  			: Generate Identify
* 
* Current Version	: 1.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2013-02-18
*/

#pragma once

#ifndef _PLIB_NETWORK_IDENTIFY_HPP_
#define _PLIB_NETWORK_IDENTIFY_HPP_

#include <Plib-Threading/Threading.hpp>
#include <Plib-Text/Text.hpp>

namespace Plib
{
	namespace Utility
	{
		class Identify
		{
		public:
			static Plib::Text::String New( )
			{
				Plib::Text::String _identify;

			#if _DEF_WIN32
				::SYSTEMTIME sysTime;
				::GetLocalTime( &sysTime );
				_identify.Format( "%04d%02d%02d%02d%02d%02d%04d+%ld", 
					sysTime.wYear, sysTime.wMonth, sysTime.wDay,
					sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
					sysTime.wMilliseconds, Plib::Threading::ThreadSys::ThreadId()
				);
			#else
				struct timeb _timeBasic;
				struct tm *  _timeStruct;
				ftime( &_timeBasic );
				_timeStruct = localtime( &_timeBasic.time );
				_identify.Format( "%04d%02d%02d%02d%02d%02d%04d+%ld", 
					(Uint16)(_timeStruct->tm_year + 1900), (Uint8)(_timeStruct->tm_mon + 1), 
					(Uint8)(_timeStruct->tm_mday), (Uint8)(_timeStruct->tm_hour), 
					(Uint8)(_timeStruct->tm_min), (Uint32)(_timeStruct->tm_sec), 
					(Uint16)(_timeBasic.millitm), Plib::Threading::ThreadSys::ThreadId()
				);
			#endif
				return _identify;
			}
		};
	}
}

#endif // plib.network.identify.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */
