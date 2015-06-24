#pragma once

#include <ostream>

namespace perseus
{
  namespace detail
  {
    /**
    @brief Values and descriptions of the opcodes.

    The description format is:

    # Name of opcode

    Short description of the opcode.

    ## Parameters (optional)

    Lists the parameters stored in the bytecode in order, if any, e.g.:

    -   Param1 (4 bytes) - description of this parameter

    ## Input (optional)

    Lists the parameters popped from the stack in the order in which they are popped, if any, e.g.:

    -   Input1 (1 byte) - description of the input on the top of the stack
    -   Input2 (varying) [left on stack] - description of the input below that on the stack, explaining how its size is determined

    ## Output (optional)

    Lists the results that will be pushed onto the stack in the order in which they are pushed, if any, e.g.:

    -   Output1 (4 bytes) - first value that will be pushed onto the stack

    ## Description (optional)

    Elaborate description of the opcode, if it's not well defined by the above.

    */
    enum class opcode : unsigned int
    {
      /**
      # No Operation

      Does nothing.
      */
      no_operation,
      /**
      # Exit

      Terminates execution, returning the stack in its current state to the environment.

      May only be called in the root coroutine, i.e. the one where execution began, to make it harder to write leaking code.

      @throws exit_in_coroutine if called in a coroutine
      */
      exit,
      /**
      */

      //    Coroutines

      /**
      # Absolute Coroutine

      Create a new coroutine by absolute address of its initial @ref instruction_pointer.

      ## Parameters

      -   absolute_address (4 bytes) - address of initial @ref instruction_pointer

      ## Output

      -   coroutine_identifier (4 bytes) - unique identifier of the new coroutine (may reuse identifier of a deleted coroutine)
      */
      absolute_coroutine,
      /**
      # Indirect Coroutine

      Create a new coroutine by absolute address of its initial @ref instruction_pointer, supplied on the stack

      ## Input

      -   absolute_address( 4 bytes) - address of initial @ref instruction_pointer

      ## Output

      -   coroutine_identifier (4 bytes) - unique identifier of the new coroutine( may reuse identifier of a deleted coroutine)

      @todo is this a sensible name? Is indirect the right word?
      */
      indirect_coroutine,
      /**
      # Resume Coroutine

      Resume execution of a given coroutine. Resuming a non-suspended coroutine or supplying an invalid identifier is a critical error.

      Pops the identifier and the data off the stack and pushes the yielded/returned data on the stack once execution passes back to the current coroutine.

      ## Parameters

      -   data_size (4 bytes) - size (in bytes) of data being passed to the coroutine being resumed

      ## Input

      -   coroutine_identifier (4 bytes) - identifier of coroutine to resume
      -   data (varying, as given in data_size) - data to push onto the coroutine's stack before resuming it

      ## Output

      -   result (varying) - whatever the coroutine returned/yielded

      @see opcode::coroutine_yield for passing execution back from the coroutine without killing it
      @see opcode::coroutine_return for passing execution back from the coroutine, killing it
      @throws invalid_coroutine_identifer if an incorrect identifier is supplied
      @throws resuming_dead_coroutine when trying to resume a dead coroutine
      @throws resuming_live_coroutine when trying to resume a live coroutine
      */
      resume_coroutine,
      /**
      # Resume Pushing Everything

      Like @ref resume_coroutine, but moving the whole stack to the coroutine. For resuming coroutines from C++.

      ## Input

      -   coroutine_identifier (4 bytes) - identifier of coroutine to resume
      -   data (varying, remaining stack) - data to push onto coroutines stack before resuming it

      ## Output

      -   result (varying) - whatever the coroutine returned/yielded

      @see opcode::resume_coroutine for exception information etc.
      */
      resume_pushing_everything,
      /**
      # Coroutine State

      Retrieve the state of a coroutine (live/suspended/dead). Supplying an invalid identifier is a critical error.

      ## Input

      -   coroutine_identifier (4 bytes) - whose coroutine to query the state of

      ## Output

      -   state (1 byte) - state of the supplied coroutine

      @see coroutine::state Returned state
      @see invalid_coroutine_identifer thrown if an incorrect identifier is supplied
      */
      coroutine_state,
      /**
      # Delete Coroutine

      Delete a given coroutine, freeing resources.

      Trying to delete a live coroutine (i.e. the active one or one waiting on it) or supplying an invalid identifier is a critical error.

      ## Input
      
      -   coroutine_identifier (4 bytes) - which coroutine to kill

      @throws deleting_live_coroutine when trying to delete an active coroutine
      @throws invalid_coroutine_identifier when supplying an invalid identifier
      */
      delete_coroutine,
      /**
      # Coroutine Return

      Terminates the current coroutine, returning execution to the coroutine that called it.

      A given number of bytes will be popped off the stack and pushed onto the parent coroutine's stack before returning it.

      When used in the root coroutine, this works like exit.

      ## Parameters

      -   data_size (4 bytes) - size of data to return

      ## Input

      -   data (varying, as given in data_size) - number of bytes from the stack to return to parent coroutine.
      */
      coroutine_return,
      /**
      # Yield

      Suspend the current coroutine, returning execution to the coroutine that called it.

      A given number of bytes will be popped off the stacked and pushed onto the parent coroutine's stack as this coroutine is suspended.

      ## Parameters

      -   data_size (4 bytes) - size of data to yield

      ## Input

      -   yield_data (varying, as given in data_size) - number of bytes from the stack to yield to parent coroutine

      ## Output

      -   resume_data (varying) - data supplied in the @ref opcode::resume_coroutine "resume_coroutine" call

      @see opcode::resume_coroutine for how execution returns to this coroutine after it has been suspended
      */
      yield,

      //    Push/Pop

      /**
      # Push 8 Bit Constant

      Pushes a given 8 bit constant onto the stack.

      ## Parameters

      -   data (1 byte) - constant to push

      ## Output

      -   data (1 byte) - the given data
      */
      push_8,
      /**
      # Push 32 Bit Constant

      Pushes a given 32 bit constant onto the stack.

      ## Parameters

      -   data (4 bytes) - constant to push

      ## Output

      -   data (4 bytes) - the given data
      */
      push_32,
      /**
      # Pop

      Pops a given number of bytes off the stack

      ## Parameters
      
      -   size (4 bytes) - number of bytes to pop

      ## Input

      -   discarded (varying, as given in size) - the data that gets popped

      @throws stack_underflow if popping more than there is on the stack
      */
      pop,
      /**
      @brief end marker
      
      Not a valid opcode, just used as an end marker.

      Useful for testing the handling of invalid opcodes.
      **/
      opcode_end
    };

    /// Maximum number of bytes for multibyte opcode encoding
    static constexpr unsigned int opcode_max_bytes = 4;

    /**
    @brief Feeding an opcode into an std::ostream
    */
    std::ostream& operator<<( std::ostream& stream, const opcode code );
  }
}
