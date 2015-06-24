#pragma once

#include <cstdint>

#include "vm/stack.hpp"
#include "vm/instruction_pointer.hpp"

namespace perseus
{
  namespace detail
  {
    // forward declaration
    class code_segment;
    
    /**
    @brief A cooperative task of execution

    A @ref processor can run multiple coroutines. 

    @see coroutine_manager responsible for creation & deletion
    @see processor responsible for execution
    */
    struct coroutine
    {
      /// coroutine identifier
      typedef std::uint32_t identifier;

      /// State of a @ref coroutine
      enum class state : unsigned char
      {
        /// The running coroutine, or one waiting for it
        live,
        /// A coroutine that may be resumed, i.e. one that was not yet started or that yielded
        suspended,
        /// A coroutine that may not be resumed, i.e. one that finished execution via @ref opcode::coroutine_return "coroutine_return".
        dead
      };

      /**
      @brief Constructor
      @param index this coroutine's index in the list of coroutines; used to reference its stack
      @param code the @ref code_segment this coroutine executes
      @param start_address where in the code this coroutine starts execution
      @param initial_stack initial state of the coroutine's stack
      */
      coroutine( const identifier index, const code_segment& code, const instruction_pointer::value_type start_address, stack&& initial_stack );
      
      /// identifier given by the @ref coroutine_manager
      const identifier index;
      /// execution @ref stack
      stack stack;
      /// the @ref instruction_pointer
      instruction_pointer instruction_pointer;
      /// the current @ref state of the coroutine
      state current_state = state::suspended;
    };
  }
}
