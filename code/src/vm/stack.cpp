#include "vm/stack.hpp"

#include <utility>

namespace perseus
{

  stack::stack( stack&& rhs )
    : std::vector< char >( std::move( rhs ) )
  {
  }

  stack& stack::operator=( stack&& rhs )
  {
    std::vector< char >::operator=( std::move( rhs ) );
    return *this;
  }

  stack stack::split( const size_type size )
  {
    if( size > this->size() )
    {
      throw stack_underflow( "Stack underflow" );
    }
    return stack( *this, size );
  }

  void stack::append( const stack& other )
  {
    insert( end(), other.begin(), other.end() );
  }

  stack::stack( stack& other, const size_type size )
    // could generally use std::make_move_iterator, but char doesn't have a faster move constructor (obviously)
    : std::vector<char>( other.end() - size, other.end() )
  {
    other.resize( other.size() - size );
  }
}
