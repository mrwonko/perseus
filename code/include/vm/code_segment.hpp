#pragma once

#include <vector>
#include <type_traits>
#include <stdexcept>
#include <climits>
#include <cassert>

#include "shared/opcodes.hpp"

namespace perseus
{
  namespace detail
  {
    /**
    @brief Memory segment containing executable bytecode.

    @todo Add support for custom allocators? (remove mention of std::bad_alloc in push then)
    */
    class code_segment : public std::vector< char >
    {
    public:
      // inherit constructors
      using std::vector< char >::vector;

      /**
      @brief Writes a value to the end of the @ref code_segment
      @tparam T type of value to push back
      @param value the value to push back
      @throws std::bad_alloc if the system is out of memory
      @todo Write bytes in reverse on big endian for cross-platform bytecode
      */
      template< typename T >
      void push( const T value )
      {
        const_pointer begin = reinterpret_cast< const_pointer >( &value );
        const_pointer end = begin + sizeof( T );
        insert( std::vector< char >::end(), begin, end );
      }
      /**
      @brief Writes an @ref opcode to the end of the @ref code_segment

      Opcodes have varying length; this function encodes the opcode properly, then stores it.
      @param code the opcode to encode and push back
      @throws std::bad_alloc if the system is out of memory
      @note The encoding is somewhat similar to UTF-8 but without marking the continuations: The number of continuous high bits in the first byte is the number of additional bytes following. (UTF-8 has an additional 1-bit.) Those additional bytes are not marked in any way (unlike UTF-8, where they begin with `10xxxxxx`). So for example `10xxxxxx` `xxxxxxxx` for a 2 byte value, containing 8 to 14 bits.
      */
      template<>
      void push< opcode >( const opcode code )
      {
        pushOpcode( code );
      }

    private:
      /// implementation of push<opcode>()
      void pushOpcode( const opcode code );
    };
  }
}
