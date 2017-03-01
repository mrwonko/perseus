#include "vm/instruction_pointer.hpp"

#include <cassert>

namespace perseus
{
  namespace detail
  {
    instruction_pointer::instruction_pointer( const code_segment& code, const value_type offset )
      : _code( &code )
      , _offset( offset )
    {
    }

    instruction_pointer& instruction_pointer::operator=( const value_type absolute_offset )
    {
      if( absolute_offset >= _code->size() )
      {
        throw code_segmentation_fault( "Moved instruction_pointer past end of code_segment!" );
      }
      _offset = absolute_offset;
      return *this;
    }

    instruction_pointer& instruction_pointer::operator+=( const difference_type relative_offset )
    {
      if( relative_offset < 0 && static_cast< difference_type >( _offset ) + relative_offset < 0 )
      {
        throw code_segmentation_fault( "Trying to move instruction_pointer before start of code_segment!" );
      }
      if( _offset + relative_offset >= _code->size() )
      {
        throw code_segmentation_fault( "Moved instruction_pointer past end of code_segment!" );
      }
      _offset += relative_offset;
      return *this;
    }

    opcode instruction_pointer::readOpcode()
    {
      typedef std::underlying_type_t< opcode > type;
      type value = 0u;

      const unsigned char first_byte = read< unsigned char >();
      if( !( first_byte & ( 1u << 7u ) ) )
      {
        // single byte
        value = first_byte;
      }
      else
      {
        // multi byte
        unsigned int continuation_count = 1; // we only get here if the highest bit is 1
        constexpr unsigned char high_bit_mask = 1u << ( CHAR_BIT - 1u );
        for( unsigned char mask = high_bit_mask >> 1u; first_byte & mask; mask >>= 1u )
        {
          ++continuation_count;
        }
        // there are continuation_count 1-bits and one 0-bit
        const unsigned char first_byte_mask = ( 1u << ( CHAR_BIT - 1u - continuation_count ) ) - 1u;
        value = static_cast< type >( first_byte & first_byte_mask );
        for( unsigned int continuation = 0u; continuation < continuation_count; ++continuation )
        {
          value <<= CHAR_BIT;
          value |= static_cast< type >( read< unsigned char >() );
        }
      }

      return static_cast< opcode >( value );
    }

    template<>
    opcode instruction_pointer::read< opcode >()
    {
      return readOpcode();
    }
  }
}
