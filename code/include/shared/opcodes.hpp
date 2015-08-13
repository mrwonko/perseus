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
      # Syscall

      Call a user-defined function.

      ## Parameters

      -   syscall_index (4 bytes) - index of syscall to execute

      ## Input

      Up to the syscall.

      ## Output

      Up to the syscall.

      @throws invalid_syscall if no such syscall exists
      */
      syscall,
      /**
      # Low Level Break

      Break into the system's debugger via DebugBreak()/raise(SIGTRAP)
      */
      low_level_break,

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
      /**
      # Push Coroutine Identifier

      Pushes the identifier of the current coroutine onto the stack. Also works for the root coroutine.

      ## Output

      -   coroutine_identifier (4 bytes)
      */
      push_coroutine_identifier,

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
      # Reserve

      Reserves zero-initialized space on top of the stack.

      ## Parameters

      -   size (4 bytes) - unsigned number of bytes to reserve

      ## Output

      -   memory (varying, of given size) - a zero-filled block of memory of the requested size
      */
      reserve,
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

      //    Load / Store

      // TODO implement the following
      // TODO test the following
      /**
      # Absolute load from current coroutine's stack

      Push (copy) a given number of bytes from a given location on the current coroutine's stack

      ## Parameters

      -   size (4 bytes) - unsigned number of bytes to copy

      ## Input

      -   address (4 bytes) - unsigned address on this coroutine's stack to read from

      ## Output

      -   bytes (varying, as given in size) - copied data

      @throws stack_segmentation_fault if attempting read from above top of stack
      */
      absolute_load_current_stack,
      /**
      # Absolute store onto current coroutine's stack

      Reads bytes from the top of the stack and writes them at the given location into the current stack.

      The read data is not removed from the stack; if that is desired, issue a discard as well.

      ## Parameters

      -   size (4 bytes) - unsigned number of bytes to pop/write

      ## Input

      -   address (4 bytes) - unsigned address on this coroutine's stack to write to (overwriting what is there)

      @throws stack_segmentation_fault if attempting write above top of stack
      */
      absolute_store_current_stack,
      /**
      # Absolute load from arbitrary coroutine's stack

      Push (copy) a given number of bytes from a given location on a given coroutine's stack

      ## Parameters

      -   size( 4 bytes) - unsigned number of bytes to copy

      ## Input

      -   address (4 bytes) - unsigned address on the coroutine's stack to read from
      -   coroutine_identifier (4 bytes) - coroutine from whose stack to read

      ## Output

      -   bytes (varying, as given in size) - copied data

      @throws stack_segmentation_fault if attempting read from above top of stack
      @see invalid_coroutine_identifer thrown if an incorrect coroutine identifier is supplied
      */
      absolute_load_stack,
      /**
      # Absolute store onto arbitrary coroutine's stack

      Reads bytes from the top of the stack and writes them at the given location into the stack of the given coroutine

      The read data is not removed from the stack; if that is desired, issue a pop as well.

      ## Parameters

      -   size (4 bytes) - unsigned number of bytes to pop/write

      ## Input

      -   address (4 bytes) - unsigned address on given coroutine's stack to write to (overwriting what is there)
      -   coroutine_identifier (4 bytes) - coroutine on whose stack to write
      -   (data (varying) - written to location, but kept on stack)

      @throws stack_segmentation_fault if attempting write above top of stack
      @see invalid_coroutine_identifer thrown if an incorrect coroutine identifier is supplied
      */
      absolute_store_stack,
      /**
      # Relative load from current coroutine's stack

      Push (copy) a given number of bytes from a given offset into the current coroutine's stack

      ## Parameters

      -   size (4 bytes) - unsigned number of bytes to copy
      -   offset (4 bytes) - signed offset into current coroutine's stack (must be negative)

      ## Output

      -   bytes (varying, as given in size) - copied data

      @throws stack_segmentation_fault if attempting read from above top of stack
      */
      relative_load_stack,
      /**
      # Relative store onto current coroutine's stack

      Reads bytes from the top of the stack and writes them at the given offset into the current stack.

      The read data is not removed from the stack; if that is desired, issue a pop as well.

      ## Parameters

      -   size (4 bytes) - unsigned number of bytes to pop/write
      -   offset (4 bytes) - signed offset into current coroutine's stack before popping the bytes (must be negative)

      @throws stack_segmentation_fault if attempting write above top of stack
      */
      relative_store_stack,
      /**
      # Push Stack Size

      Retrieve the current size of this coroutine's stack, i.e. the address on top.

      ## Output

      -   size (4 bytes) - unsigned size of current coroutine's stack
      */
      push_stack_size,

      //    Jumps/Calls

      /**
      # Absolute Jump

      Set the Instruction Pointer to the given address

      ## Parameters

      -   address (4 bytes) - unsigned location to jump to

      @throws code_segmentation_fault if jumping outside valid code
      */
      absolute_jump,
      /**
      # Relative Jump

      Add the given offset to the Instruction Pointer (after it has been moved past this instruction).

      ## Parameters

      -   offset (4 bytes) - signed offset to change the instruction pointer by

      @throws code_segmentation_fault if jumping outside valid code
      */
      relative_jump,
      /**
      # Relative Jump if false

      Do the given relative jump if the value on the stack is false

      ## Parameters

      -   offset (4 bytes) - signed offset to change instruction pointer by, if value on stack is false

      ## Input

      -   value (1 byte) - value determining whether the jump is made; it is made iff it is false (0)

      @throws code_segmentation_fault if jumping outside valid code
      */
      relative_jump_if_false,
      /**
      # Indirect Jump

      Set the Instruction Pointer to a location supplied on the stack

      ## Input

      -   address (4 bytes) - unsigned location to jump to

      @throws code_segmentation_fault if jumping outside valid code
      */
      indirect_jump,
      /**
      # Call

      Pushes the address after this instruction onto the stack, then jumps to the given address.

      Calling convention is to first make space for the return value on the stack, then push the arguments, finally call. Callee pops the arguments.

      ## Parameters

      -   address (4 bytes) - unsigned location to jump to

      ## Output

      -   return_address (4 bytes) - address after this instruction so the called function can return there

      @throws code_segmentation_fault if jumping outside valid code
      */
      call,
      /**
      # Indirect Call

      Pushes the address after this instruction onto the stack, then jumps to the address supplied on the stack.

      Same as call, except the address is supplied using the stack instead of the code.

      ## Input

      -   address (4 bytes) - unsigned location to jump to

      ## Output

      -   return_address (4 bytes) - address after this instruction so the called function can return there

      @throws code_segmentation_fault if jumping outside valid code
      */
      indirect_call,
      /**
      # Return

      Pop the return address and arguments from the stack, then jump to the return address

      ## Parameters

      -   argument_size (4 bytes) - the number of bytes to pop off the stack in addition to the return address

      ## Input

      -   return address (4 bytes) - unsigned location to jump to
      -   arguments (varying, as given in argument_size) - arguments of this function to now discard

      @throws code_segmentation_fault if jumping outside valid code
      */
      return_,

      //    Boolean

      /**
      # Boolean And

      ## Input

      -   operand2 (1 byte)
      -   operand1 (1 byte)

      ## Output

      -   result (1 byte) - operand1 && operand2
      */
      and_b,
      /**
      # Boolean Or

      ## Input

      -   operand2 (1 byte)
      -   operand1 (1 byte)

      ## Output

      -   result (1 byte) - operand1 || operand2
      */
      or_b,
      /**
      # Boolean Equals

      ## Input

      -   operand2 (1 byte)
      -   operand1 (1 byte)

      ## Output

      -   result (1 byte) - operand1 == operand2
      */
      equals_b,
      /**
      # Boolean Not Equals

      ## Input

      -   operand2 (1 byte)
      -   operand1 (1 byte)

      ## Output

      -   result (1 byte) - operand1 != operand2
      */
      not_equals_b,
      /**
      # Boolean And

      ## Input

      -   operand (1 byte)

      ## Output

      -   result (1 byte) - !operand
      */
      negate_b,

      //    32 bit signed integer

      /**
      # 32 bit signed int addition

      ## Input

      -   operand2 (4 bytes)
      -   operand1 (4 bytes)

      ## Output

      -   result (4 bytes) - operand1 + operand2
      */
      add_i32,
      /**
      # 32 bit signed int subtraction

      ## Input

      -   operand2 (4 bytes)
      -   operand1 (4 bytes)

      ## Output

      -   result (4 bytes) - operand1 - operand2
      */
      subtract_i32,
      /**
      # 32 bit signed int multiplication

      ## Input

      -   operand2 (4 bytes)
      -   operand1 (4 bytes)

      ## Output

      -   result (4 bytes) - operand1 * operand2
      */
      multiply_i32,
      /**
      # 32 bit signed int division

      Rounded down.

      ## Input

      -   operand2 (4 bytes)
      -   operand1 (4 bytes)

      ## Output

      -   result (4 bytes) - operand1 / operand2

      @throws divide_by_zero if operand2 is 0
      */
      divide_i32,
      /**
      # 32 bit signed int equals

      ## Input

      -   operand2 (4 bytes)
      -   operand1 (4 bytes)

      ## Output

      -   result (1 byte) - operand1 == operand2
      */
      equals_i32,
      /**
      # 32 bit signed int not equals

      ## Input

      -   operand2 (4 bytes)
      -   operand1 (4 bytes)

      ## Output

      -   result (1 byte) - operand1 != operand2
      */
      not_equals_i32,
      /**
      # 32 bit signed int less than

      ## Input

      -   operand2 (4 bytes)
      -   operand1 (4 bytes)

      ## Output

      -   result (1 byte) - operand1 < operand2
      */
      less_than_i32,
      /**
      # 32 bit signed int less than or equals

      ## Input

      -   operand2 (4 bytes)
      -   operand1 (4 bytes)

      ## Output

      -   result (1 byte) - operand1 <= operand2
      */
      less_than_or_equals_i32,
      /**
      # 32 bit signed int greater than

      ## Input

      -   operand2 (4 bytes)
      -   operand1 (4 bytes)

      ## Output

      -   result (1 byte) - operand1 > operand2
      */
      greater_than_i32,
      /**
      # 32 bit signed int greater than or equals

      ## Input

      -   operand2 (4 bytes)
      -   operand1 (4 bytes)

      ## Output

      -   result (1 byte) - operand1 >= operand2
      */
      greater_than_or_equals_i32,
      /**
      # 32 bit signed int negation

      ## Input

      -   operand (4 bytes)

      ## Output

      -   result (4 bytes) - (-operand1)
      */
      negate_i32,
      /**
      # 32 bit signed int modulo

      ## Input

      -   operand2 (4 bytes)
      -   operand1 (4 bytes)

      ## Output

      -   result (4 bytes) - operand1 % operand2

      @throws divide_by_zero if operand2 is 0
      */
      modulo_i32,

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
