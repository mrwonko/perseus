#pragma once

#include <ostream>
#include <string>
#include <ios>

namespace std
{
  static std::ostream& operator<<( std::ostream& os, const std::u32string& s )
  {
    for( const char32_t& c : s )
    {
      if( c < 128 )
      {
        os << char( c );
      }
      else
      {
        os << "<U+" << std::hex << c << ">";
      }
    }
    return os;
  }
}
