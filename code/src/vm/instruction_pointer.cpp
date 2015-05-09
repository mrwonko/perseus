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
  }
}
