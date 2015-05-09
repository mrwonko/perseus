#pragma once

#include <climits>
#include <type_traits>

#include "shared/opcodes.hpp"
#include "vm/code_segment.hpp"
#include "vm/exceptions.hpp"

namespace perseus
{
  namespace detail
  {
    /**
    @brief Pointer into a @ref code_segment for reading values.

    Has special handling for reading @ref opcode "opcodes" since they are of varying length.
    */
    class instruction_pointer
    {
    public:
      /**
      @brief Constructor

      @param code @ref code_segment this @ref instruction_pointer points into
      @param offset offset into the @ref code_segment this @ref instruction_pointer starts at
      */
      instruction_pointer( const code_segment& code, const code_segment::size_type offset = 0 );

      /// Copy constructor
      instruction_pointer( const instruction_pointer& ) = default;
      /// Copy assignment
      instruction_pointer& operator=( const instruction_pointer& ) = default;
      /// Move constructor
      instruction_pointer( instruction_pointer&& ) = default;
      /// Move assignment
      instruction_pointer& operator=( instruction_pointer&& ) = default;

      /**
      @brief Reads a value from the @ref code_segment, incrementing the pointer by its size.
      @tparam T type of value to read
      @returns The read value
      @throws code_segmentation_fault if the read goes past the end of the @ref code_segment.
      */
      template< typename T >
      T read()
      {
        const T* result = reinterpret_cast< const T* >( _code->data() + _offset );
        _offset += sizeof( T );
        if( _offset > _code->size() )
        {
          throw code_segmentation_fault( "instruction_pointer read past end of code_segment!" );
        }
        return *result;
      }

      /**
      @brief Read an @ref opcode from the @ref code_segment, incrementing the pointer by its size.

      Opcodes have varying size (encoded); this specialization handles that.

      @returns @ref opcode pointed to by this @ref instruction_pointer, decoded.
      @throws code_segmentation_fault if the read goes past the end of the @ref code_segment.
      @see code_segment::push<opcode>() for information on the encoding
      */
      template<>
      opcode read< opcode >()
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

    private:
      /// Pointer to the code segment we point into
      const code_segment* _code;
      /// Offset into _code
      code_segment::size_type _offset;
    };
  }
}
