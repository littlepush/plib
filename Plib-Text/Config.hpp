/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: Config.hpp
* Propose  			: Rewrite the old ioconfig, use new String and ArrayList.
* 
* Current Version	: 1.1
* Change Log		: Re-Write..
* Author			: Push Chen
* Change Date		: 2011-07-02
*/

#pragma once

#ifndef _PLIB_TEXT_CONFIG_HPP_
#define _PLIB_TEXT_CONFIG_HPP_

#include <Plib-Text/String.hpp>
#include <Plib-Text/File.hpp>

#include <map>

namespace Plib
{
	namespace Text
	{
		typedef enum {
			CFG_COMMENT,		// Comment start chars
			CFG_KVSPILT,		// Key-value split chars
			CFG_MVWRAP,			// Multiple value wrap chars.
			CFG_MVSPILT			// Multiple value split chars.
		} CFG_PARAM;
		
		/*
		 * Configure file parser class.
		 * One can define the comment type, key-value split character,
		 * multiple value wrap character, multiple value split character.
		 */
		class Config_
		{			
		protected:
			String												m_Params[4];
			std::map< String, String > 							m_KeyValuePair;
			std::map< String, Plib::Generic::Array< String > > 	m_KeyArrayValue;

			String 												_LastKey;
			Int32 												_bArrayContinue;
			bool 												_bUnCompleteLine;
			
		protected:
			bool __ParseIncomingString( String & _lineString ) {
				// For each line
				// is comment
				_lineString.Trim( );		// Trim to be processed.
				if ( _lineString.size() == 0 ) return true;	// empty line.
				
				if ( m_Params[CFG_COMMENT].Find( _lineString[0] ) != String::NoPos )
					return true;	// this line is a comment line.
				if ( _bArrayContinue > 0 ) { // Check the array data.
					if ( _lineString.Find(m_Params[CFG_MVWRAP][0]) != String::NoPos )
						SELF_INCREASE( _bArrayContinue );
					if ( _lineString[_lineString.size() - 1] == m_Params[CFG_MVWRAP][1] ) {
						SELF_DECREASE( _bArrayContinue );
						if ( _bArrayContinue == 0 ) {
							_lineString.Remove( _lineString.Size() - 1 );
							if ( _lineString.Empty( ) ) return true;
						}
					}
					if ( m_Params[CFG_MVSPILT].Find(
						_lineString[_lineString.size() - 1])
						== String::NoPos )
					{
						_lineString += m_Params[CFG_MVSPILT][0];
					}
					m_KeyValuePair[ _LastKey ] += _lineString;
					return true;
				}
				// Get the Key.
				Uint32 _pos = String::NoPos;
				for ( Uint32 i = 0; i < m_Params[CFG_KVSPILT].size(); ++i ) {
					Uint32 _kvPos = _lineString.Find( m_Params[CFG_KVSPILT][i] );
					_pos = ( _kvPos < _pos ) ? _kvPos : _pos;
				}
				String _Key = _lineString.SubString( 0, _pos );
				if ( _Key.Trim().Size() == 0 ) {
					// No Key
					if ( _bUnCompleteLine == false ) // Error Here
					{
						return false;
					}
				} else {
					if ( _bUnCompleteLine == true ) // Error Here, Last line is uncomplete
					{
						return false;
					}
					_LastKey.DeepCopy( _Key.Trim() );	// backup the last key.
				}
				if ( _pos == String::NoPos ) {
					// No Value in this line.
					_bUnCompleteLine = true;
					return true;
				}
				String _Value = _lineString.SubString( _pos + 1 );
				_Value.Trim();
				if ( _Value[0] == m_Params[CFG_MVWRAP][0] ) {
					_Value.Remove( 0 );
					SELF_INCREASE(_bArrayContinue);
					if ( _Value.Size() == 0 ) return true;
					if ( _Value[_Value.size() - 1] == m_Params[CFG_MVWRAP][1] ) {
						_Value.Remove( _Value.size() - 1 );
						SELF_DECREASE(_bArrayContinue);
					}
				}
				m_KeyValuePair[ _LastKey ] = _Value;
				return true;
			}
			
		public:
			// Default C'str
			Config_( 
				String _comment = "#", 
				String _kvsplit = "=", 
				String _mvwrap = "{}", 
				String _mvsplit = ";," )
				: _bArrayContinue( 0 ), _bUnCompleteLine( false )
			{
				CONSTRUCTURE;
				m_Params[CFG_COMMENT].DeepCopy(_comment);
				m_Params[CFG_KVSPILT].DeepCopy(_kvsplit);
				m_Params[CFG_MVWRAP].DeepCopy(_mvwrap);
				m_Params[CFG_MVSPILT].DeepCopy(_mvsplit);
			}
			~Config_( ) { DESTRUCTURE; }
			
			// Set specified characters.
			void Set( CFG_PARAM _param, String _data )
			{
				m_Params[_param].DeepCopy( _data );
			}
			
			// Clear last parse result.
			void Clear( )
			{
				m_KeyValuePair.clear();
				m_KeyArrayValue.clear();
				_bArrayContinue = 0;
				_bUnCompleteLine = false;
			}
			
			// Parse the input array to be a config object.
			bool Parse( Plib::Generic::Array< String > & _LineArray )
			{
				Clear(); 
				for ( Uint32 i = 0; i < _LineArray.Size(); ++i )
				{
					if ( !__ParseIncomingString( _LineArray[i] ) )
						return false;
				}
				return true;
			}
			
			// Parse the incoming string.
			bool Parse( String & _stringConfig )
			{
				Plib::Generic::Array< String > _LineArray = 
					_stringConfig.Split("\r\n" + m_Params[CFG_MVSPILT]);
				return Parse( _LineArray );
			}
			
			// Open the file and parse it.
			bool ParseConfigFile( const String & _filePath )
			{
				Clear();

				// Load the config file
				ReadStream rsConfig( _filePath );
				for ( ; ; ) {
					String _line = rsConfig.ReadLine( );
					if ( _line == String::Null ) break;
					if ( __ParseIncomingString( _line ) == false )
						return false;
				}
				
				// Check if the configure file is well formated.
				if ( _bArrayContinue > 0 ) return false;
				return true;
			}
			
			// Random access the data.			
			String operator [] ( const char * _key ) {
				return m_KeyValuePair[ String( _key ) ];
			}
			// String version.
			String operator [] ( const String & _key ) {
				return m_KeyValuePair[_key];
			}
			// Get specified value
			String Get( const char * _key ) {
				return m_KeyValuePair[ String( _key ) ];
			}
			// String Version.
			String Get( const String & _key ) {
				return m_KeyValuePair[_key];
			}
			// Get an array
			Plib::Generic::Array<String> GetArray( const char * _key ) {
				return GetArray( String( _key ) );
			}
			// String Version.
			Plib::Generic::Array<String> GetArray( const String & _key )
			{
				if ( m_KeyArrayValue.find( _key ) != m_KeyArrayValue.end() )
					return m_KeyArrayValue[_key];
				String _srcValue = this->Get( _key );
				Plib::Generic::Array< String > _rArray = _srcValue.Split( m_Params[CFG_MVSPILT] );
				m_KeyArrayValue[_key] = _rArray;
				return _rArray;
			}
			
			// Check if contains specified key.
			INLINE bool ContainsKey( const char * _key ) const {
				return m_KeyValuePair.find( String(_key) ) != m_KeyValuePair.end();
			}
			INLINE bool ContainsKey( const String & _key ) const {
				return m_KeyValuePair.find( _key ) != m_KeyValuePair.end();
			}
		};
		
		// Referernce Version of Config_
		template < Uint32 _dummy >
		class Config_r : public Plib::Generic::Reference< Config_ >
		{
			typedef Plib::Generic::Reference< Config_ >		TFather;
		protected:
			Config_r<_dummy>( bool _beNull ) : TFather( false ) { CONSTRUCTURE; }
		public:
			// C'Str
			Config_r<_dummy>( ) : TFather( true ) { CONSTRUCTURE; }
			Config_r<_dummy>( const Config_r<_dummy> & rhs ) : TFather( rhs ) { CONSTRUCTURE; }
			~Config_r<_dummy>( ) { DESTRUCTURE; }
			// Open a configure file.
			Config_r<_dummy>( const String & _filePath ) : TFather( true ) {
				CONSTRUCTURE;
				TFather::_Handle->_PHandle->ParseConfigFile( _filePath );
			}
			// Clear last parse result.
			INLINE void Clear( ) { TFather::_Handle->_PHandle->Clear( ); }
			// Set specified characters.
			INLINE void Set( CFG_PARAM _param, String _data ) {
				TFather::_Handle->_PHandle->Set( _param, _data );
			}
			// Parse the input array to be a config object.
			INLINE bool Parse( Plib::Generic::Array< String > & _LineArray ) {
				return TFather::_Handle->_PHandle->Parse( _LineArray );
			}
			// Parse the incoming string.
			INLINE bool Parse( String & _stringConfig ) {
				return TFather::_Handle->_PHandle->Parse( _stringConfig );
			}
			// Open the file and parse it.
			INLINE bool ParseConfigFile( const String & _filePath ) {
				return TFather::_Handle->_PHandle->ParseConfigFile( _filePath );
			}
			// Random access the data.			
			String operator [] ( const char * _key ) {
				return TFather::_Handle->_PHandle->operator [] ( _key );
			}
			String operator [] ( const String & _key ) {
				return TFather::_Handle->_PHandle->operator [] ( _key );
			}
			// Get array
			Plib::Generic::Array<String> GetArray( const char * _key ) {
				return TFather::_Handle->_PHandle->GetArray( _key );
			}
			// String Version.
			Plib::Generic::Array<String> GetArray( const String & _key ) {
				return TFather::_Handle->_PHandle->GetArray( _key );
			}
			
			// Check if contains specified key
			INLINE bool ContainsKey( const char * _key ) const {
				return TFather::_Handle->_PHandle->ContainsKey( _key );
			}
			INLINE bool ContainsKey( const String & _key ) const {
				return TFather::_Handle->_PHandle->ContainsKey( _key );
			}
			// Get another config
			Config_r<_dummy> GetConfig( const char * _key ) {
				Config_r<_dummy> _2ndConfig;
				Plib::Generic::Array< String > _array = 
					TFather::_Handle->_PHandle->GetArray(_key);
				_2ndConfig.Parse( _array );
				return _2ndConfig;
			}
			Config_r<_dummy> GetConfig( const String & _key ) {
				Config_r<_dummy> _2ndConfig;
				Plib::Generic::Array< String > _array = 
					TFather::_Handle->_PHandle->GetArray(_key);
				_2ndConfig.Parse( _array );
				return _2ndConfig;
			}
			
			// Null Object
			static Config_r<_dummy>			Null;
			static Config_r<_dummy> CreateNullConfig( ) {
				return Config_r<_dummy>( false );
			}
		};
		
		template < Uint32 _dummy >
		Config_r<_dummy>	Config_r<_dummy>::Null( false );
		typedef Config_r<0>	Config;
	}
}

#endif // plib.text.config.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */

