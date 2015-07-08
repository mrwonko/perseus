#include "compiler/conversions.hpp"

#include <boost/spirit/include/qi_expect.hpp>

#include <cassert>
#include <algorithm>
#include <locale> // std::locale, std::codecvt, std::use_facet
#include <cwchar> // std::mbstate_t
#include <codecvt>
#include <string>
#include <climits>
#include <string>

namespace perseus
{
  namespace detail
  {
    using namespace std::string_literals;

    char32_t readCodePoint( enhanced_istream_iterator& it, const enhanced_istream_iterator& end )
    {
      // look into std::codecvt_utf8<char32_t>?
      const unsigned char& byte0 = *it;
      if( ( byte0 & 0b1000'0000 ) == 0b0000'0000 ) // 1 byte; <= U+007F
      {
        return byte0;
      }
      // catch stray continuations
      else if( ( byte0 & 0b1100'0000 ) == 0b1000'0000 )
      {
        throw invalid_utf8( "not valid UTF-8: stray continuation", it.get_position() );
      }
      else // multibyte
      {
        // number of bytes is [number of consecutive 1 bits] - 1, up to 4
        // the two top bytes must be set if we reached this, so just check the rest
        unsigned char mask = 0b0010'0000;
        unsigned int numBytes = 2;
        for( ; numBytes < 4; ++numBytes )
        {
          if( !( byte0 & mask ) )
          {
            break;
          }
          mask >>= 1;
        }
        if( numBytes == 4 )
        {
          // byte1 must not start with 11111, as per RFC 3629 code points stop at U+10FFFF, which fits into 4 bytes
          if( byte0 & mask )
          {
            throw invalid_utf8( "not valid UTF-8: >4 byte code point!", it.get_position() );
          }
        }
        char32_t out = byte0 & ( mask - 1 );
        for( unsigned int index = 1; index < numBytes; ++index )
        {
          ++it;
          if( it == end )
          {
            throw invalid_utf8( "not valid UTF-8: unexpected end of string during multi-byte sequence!", it.get_position() );
          }
          const unsigned char& continuation = *it;
          if( ( continuation & 0b1100'0000u ) != 0b1000'0000u )
          {
            throw invalid_utf8( "not valid UTF-8: expected continuation!", it.get_position() );
          }
          out = ( out << 6 ) | ( continuation & 0b0011'1111u );
        }
        if( out > U'\U0010FFFF' )
        {
          throw invalid_utf8( "not valid UTF-8: code point larger than U+10FFFF!", it.get_position() );
        }
        // in the interest of being able to store invalid UTF16 (which Windows filenames may contain), we don't check for this
        /*
        if( out >= U'\uD800' && out <= U'\uDFFF' )
        {
          throw invalid_utf8( "not valid UTF-8: contains UTF-16 surrogate half!", it.get_position() );
        }
        */
        return out;
      }
    }
  }
}
