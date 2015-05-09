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

      Terminates execution.
      */
      exit,
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
