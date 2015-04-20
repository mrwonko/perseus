# Perseus VM

*   Stack-based
*   Custom byte code

## memory types

*   stack segment
    *   one per coroutine
    *   contains stack frames, very much like the C stack
    *   is raw memory, as in C, i.e. no explicit type information
*   code segment
    *   single continuous memory block
        *   or should it be one per module? not all that relevant yet since we only have one module at the moment
*   no data segment
    *   because no global variables
        *   so no static class members? no static variables?
*   no thread local storage, since we're single-threaded

### think about

*   stack frames - external vs embedded in stack
*   exception handling - needs information about entered scoped

## opcodes

Operands are implicity taken from the top of the stack.

Opcodes are suffixed with their type, e.g. `_((i|u)(8|16|32|64)|(f(32|64))|b)`. This is also how a boolean and (`and_b`) is differentiated from a bitwise one (e.g. `and_i32`)

Initially, only 32 bit signed types are supported.

Parameters are 32 bit, unless otherwise stated.

Machine instruction operands are pushed left to right.

*   `exit`
*   jumps; choose bit patterns such that unified handling is possible
    *   `absolute_jump <address>`
    *   `relative_jump <offset>`
    *   `relative_jump_if_true <offset>`
    *   `relative_jump_if_false <offset>`
    *   `indirect_jump` (useful for tail recursion optimization of function pointers)
    *   indexed jumps could be added later
    *   `call <address>`
    *   `indirect_call` (function pointers)
*   one each for each size of integer and float:
    *   binary operator:
        *   `add`
        *   `subtract`
        *   `multiply`
        *   `divide`
        *   `equals`
        *   `not_equals`
        *   `less_than`
        *   `less_than_or_equals`
        *   `greater_than`
        *   `greater_than_or_equals`
    *   unary operator:
        *   `invert`
*   one for each size of integer:
    *   binary operator:
        *   `modulo`
*   bitwise operators (including shifts) could conceivably be added later but would only make sense on unsigned types
*   boolean:
    *   binary operator:
        *   `and`
        *   `or`
        *   `equals`
        *   `not_equals`
    *   unary operator:
        *   **negate**
*   one for each size of data (`8|16|32|64`)
    *   `duplicate <offset>`: offset is negative from stack top, e.g. `dup 1` duplicates the byte below the top byte on the stack onto the top
*   `dup <size> <offset>`: size is in bytes, offset as above
*   `pop [<size>]`
*   there are explicitly _no type casts between signed and unsigned and between types of varying size_. These are prone to overflows, underflows and other potentially undefined or weird behaviour. If the user really needs them, he can provide a conversion function via the FFI. (This may turn out to be a horrible idea, in which case it will be revisited.)

## functions

Arguments are evaluated and pushed left to right. Callee removes them from the stack, enabling potential tail recursion optimimzation.