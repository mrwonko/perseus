#pragma once

#include "exceptions.hpp"

#include <vector>

namespace perseus
{
  /**
  @brief A coroutine's stack.

  Most operations use the stack for their parameters and results, function parameters and return values go there as well as return addresses and local variables.

  @todo Add support for custom allocators? (remove mention of std::bad_alloc in push_back() then)
  */
  class stack : private std::vector< char >
  {
  public:
    /// Constructor for empty stack
    stack() = default;
    /// Destructor
    ~stack() = default;
    /// Move constructor
    stack( stack&& );
    /// Move assignment
    stack& operator=( stack&& );
    /// non-copyable
    stack( const stack& ) = delete;
    /// non-copyable
    stack& operator=( const stack& ) = delete;

    using std::vector< char >::size;
    using std::vector< char >::reserve;
    using std::vector< char >::empty;
    using std::vector< char >::size_type;
    using std::vector< char >::clear;

    /**
    @brief Push an arbitrary value onto the stack
    @tparam T type of value to push back. Should probably be a POD type since it's copied bitwise.
    @param value what to push onto the top of the stack
    @throws std::bad_alloc if the system is out of memory
    */
    template< typename T >
    void push( const T& value )
    {
      // this code is copied from code_segment::push() - it could be factored out, but that seems like overkill to me
      const_pointer begin = reinterpret_cast< const_pointer >( &value );
      const_pointer end = begin + sizeof( T );
      insert( std::vector< char >::end(), begin, end );
    }

    /**
    @brief Pop the topmost value off the stack
    @tparam T type of value to pop. You are highly encouraged to pop only the type that was previously pushed.
    @returns The top of the stack
    @throws stack_underflow if the stack is empty (or too small to contain an object of the requested size). The stack remains unchanged.
    */
    template< typename T >
    T pop()
    {
      if( size() < sizeof( T ) )
      {
        throw stack_underflow( "Stack underflow" );
      }
      const T top = *reinterpret_cast< const T* >( data() + size() - sizeof( T ) );
      resize( size() - sizeof( T ) );
      return top;
    }

    /**
    @brief Create a new stack by popping a given size off this one.

    The order on the new stack will be the same as on the current.

    This stack will shrink by the given size.
    @param size number of bytes to @ref pop() onto the new stack
    @returns A new stack of the given size, containing the former top of this stack
    @throws stack_underflow if the given size is bigger than this stack's size(). The stack remains unchanged.
    @throws std::bad_alloc if the system is out of memory
    */
    stack split( const size_type size );

    /**
    @brief Push the contents of another stack onto this one
    @param other stack to push onto this one (in order)
    @throws std::bad_alloc if the system is out of memory
    */
    void append( const stack& other );

  private:
    /**
    @brief creates a stack by taking the top off another one
    @note used by split()
    @pre `other.size() >= size` (not checked; that check is in split())
    */
    stack( stack& other, const size_type size );
  };
}
