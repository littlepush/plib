/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: convert.hpp
* Propose  			: Object Convert Class Definition.
* 
* Current Version	: 1.1
* Change Log		: Redefinition Use RPString.
* Author			: Push Chen
* Change Date		: 2011-01-09
*/

#ifndef _PLIB_UTILITY_CONVERT_HPP_
#define _PLIB_UTILITY_CONVERT_HPP_

#include <Plib-Text/String.hpp>
#include <sstream>

namespace Plib
{
	namespace Text
	{
		// Object Convert Structure.
		struct Convert
		{
			static INLINE Plib::Text::String ToString( const char * _pData ) {
				//return String( _pData );
				return _pData;
			}
			
			template < typename _TyPoint >
			static INLINE Plib::Text::String ToString( const _TyPoint * _pPoint ) {
				return (_pPoint == NULL ) ? "<NULL>" : Plib::Text::String::Parse( "%p", _pPoint );
			}

			template < typename _Ty >
			static INLINE Plib::Text::String ToString( const _Ty & item ) {
				//std::string << item
				std::ostringstream _s;
				_s << item;
				return Plib::Text::String( _s.str().c_str() );
			}

			static INLINE Plib::Text::String ToString( Uint8 _int ) {
				return Plib::Text::String::Parse( "%u", (Uint8)_int );
			}
			static INLINE Plib::Text::String ToString( Uint16 _int ) {
				return Plib::Text::String::Parse( "%u", (Uint16)_int );
			}
			static INLINE Plib::Text::String ToString( Uint32 _int ) {
				return Plib::Text::String::Parse( "%u", (Uint32)_int );
			}
			static INLINE Plib::Text::String ToString( Uint64 _int ) {
				return Plib::Text::String::Parse( "%llu", (Uint64)_int );
			}

			static INLINE Plib::Text::String ToString( Int8 _int ) {
				return Plib::Text::String::Parse( "%d", (Int8)_int );
			}
			static INLINE Plib::Text::String ToString( Int16 _int ) {
				return Plib::Text::String::Parse( "%d", (Int16)_int );
			}
			static INLINE Plib::Text::String ToString( Int32 _int ) {
				return Plib::Text::String::Parse( "%d", (Int32)_int );
			}
			static INLINE Plib::Text::String ToString( Int64 _int ) {
				return Plib::Text::String::Parse( "%lld", (Int64)_int );
			}
			// For Thread Id.
		#if _DEF_WIN32
			static INLINE Plib::Text::String ToString( long _int ) {
				return Plib::Text::String::Parse( "%ld", _int );
			}
		#else
			static INLINE Plib::Text::String ToString( pthread_t _int ) {
				return Plib::Text::String::Parse( "%ld", (long)_int );
			}
		#endif
			static INLINE Plib::Text::String ToString( double _int ) {
				return Plib::Text::String::Parse( "%lf", _int );
			}

			static INLINE Plib::Text::String ToString( time_t _Time ) {
				struct tm *  _timeStruct;
				_timeStruct = localtime( &(_Time) );
				return Plib::Text::String::Parse( "%04d-%02d-%02d %02d:%02d:%02d", 
					(Int32)_timeStruct->tm_year + 1900, (Int32)_timeStruct->tm_mon + 1, 
					(Int32)_timeStruct->tm_mday, (Int32)_timeStruct->tm_hour, 
					(Int32)_timeStruct->tm_min, (Int32)_timeStruct->tm_sec
					);
			}

			static INLINE Plib::Text::String ToString( const Plib::Text::String & _String ) {
				return _String;
			}

			static INLINE Plib::Text::String ToString( const std::string & _String ) {
				return Plib::Text::String( _String.c_str(), _String.size() );
			}
			
			static INLINE Plib::Text::WString ToString( const std::wstring & _WString ) {
				return Plib::Text::WString( _WString.c_str(), _WString.size() );
			}

			static INLINE Plib::Text::String ToString( char _C ) {
				return Plib::Text::String( _C );
			}

			static INLINE Plib::Text::String ToString( bool _B ) {
				return _B ? "True" : "False";
			}

			// Convert the string to int/uint.
			static INLINE Uint32 ToUint32(const Plib::Text::String & _intVal){
				return (Uint32)atoi(_intVal.C_Str());
			}
			static INLINE Uint32 ToUint32(const char * _intVal){
				return (Uint32)atoi(_intVal);
			}
			static INLINE Int32 ToInt32(const Plib::Text::String & _intVal){
				return atoi(_intVal.C_Str());
			}
			static INLINE Int32 ToInt32(const char * _intVal){
				return atoi(_intVal);
			}
			static INLINE Int64 ToInt64(const char * _intVal) {
	#if _DEF_WIN32
				return _atoi64(_intVal);
	#else
				return atoll(_intVal);
	#endif
			}
			static INLINE Int64 ToInt64(const Plib::Text::String & _intVal){
				return ToInt64(_intVal.C_Str());
			}
			static INLINE Uint64 ToUint64(const char * _intVal){
				return (Uint64)ToInt64(_intVal);
			}
			static INLINE Uint64 ToUint64(const Plib::Text::String & _intVal){
				return (Uint64)ToInt64(_intVal.C_Str());
			}

			// To Double
			static INLINE double ToDouble(const char * _dbVal) {
				return atof(_dbVal);
			}
			static INLINE double ToDouble(const Plib::Text::String & _dbVal){
				return atof(_dbVal.C_Str());
			}

			// Convert the string to Boolean.
			// Either TRUE or !0 can be true.
			// "_Bool" is some how a keyword in Mac OS X..FML.
			// So I changed it to _SBool, which means "Source Bool Value"
			static INLINE bool ToBool( const Plib::Text::String & _SBool ) {
				Plib::Text::String _TBool;
				_TBool.DeepCopy(_SBool);
				_TBool.Trim().ToLower();
				if ( _TBool == "false" ) return false;
				return true;
			}

			// Convert unsigned long to IP Address
			static INLINE Plib::Text::String ToIP( const unsigned long _addr ){
				return Plib::Text::String::Parse("%u.%u.%u.%u", 
					(unsigned int)(_addr >> (0 * 8)) & 0x00FF,
					(unsigned int)(_addr >> (1 * 8)) & 0x00FF,
					(unsigned int)(_addr >> (2 * 8)) & 0x00FF,
					(unsigned int)(_addr >> (3 * 8)) & 0x00FF );
			}

		};
	}
}

#endif // plib.utility.convert.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */
