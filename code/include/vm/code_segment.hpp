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
      @note The encoding is somewhat similar to UTF-8 but without marking the continuations: The number of continuous high bits in the first byte is the number of additional bytes following. (UTF-8 has an additional 1-bit.) Those additional bytes are not marked in any way (unlike UTF-8, where they begin with `10xxxxxx`). So for example `10xxxxxx` `xxxxxxxx` for a 2 byte value, containing 8 to 14 bits.
      */
      template<>
      void push< opcode >( const opcode code )
      {
        typedef std::underlying_type_t< opcode > type;
        type value = static_cast< type >( code );

        // 1 byte sequence
        if( value < ( 1u << 7u ) )
        {
          push< unsigned char >( value );
          return;
        }

        // multi byte sequence

        // the compiler might just unroll these loops for us :)
        // try each length, use the shortest one possible
        for( unsigned int byte_count = 2; byte_count <= opcode_max_bytes; ++byte_count )
        {
          // CHAR_BIT bits in total, of which the marker uses one for the separating 0 and 1 for each continuation in the sequence
          const unsigned int marker_bits = CHAR_BIT - byte_count;
          const unsigned int continuation_count = byte_count - 1;
          const unsigned int bits = marker_bits + continuation_count * CHAR_BIT;

          // does the value fit in this length?
          if( value < ( 1u << bits ) )
          {
            //    High Byte

            unsigned char high_byte = value >> ( continuation_count * CHAR_BIT );

            constexpr unsigned char high_bit_mask = 1u << ( CHAR_BIT - 1u );
            // set the continuation_count highest bits to 1
            for( unsigned char mask = high_bit_mask; mask > ( high_bit_mask >> continuation_count ); mask >>= 1 )
            {
              assert( !( high_byte & mask ) );
              high_byte |= mask;
            }
            assert( !( high_byte & ( 0b10000000u >> continuation_count ) ) );
            push< unsigned char >( high_byte );

            //    Continuations
            for( unsigned int continuation = 0u; continuation < continuation_count; ++continuation )
            {
              constexpr unsigned int char_mask = ( 1u << CHAR_BIT ) - 1u;
              const unsigned int bytes_left = continuation_count - 1u - continuation;
              const unsigned char byte = ( value >> ( bytes_left * CHAR_BIT ) ) & char_mask;
              push< unsigned char >( byte );
            }

            //    done
            return;
          }
        }
        throw std::domain_error( "opcode too large to encode" );
      }
    };
  }
}
