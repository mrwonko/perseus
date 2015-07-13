#include "vm/code_segment.hpp"

namespace perseus
{
  namespace detail
  {
    void code_segment::pushOpcode( const opcode& code )
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
          for( unsigned char mask = high_bit_mask; mask >( high_bit_mask >> continuation_count ); mask >>= 1 )
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
  }
}
