# Perseus VM

*   Stack-based
*   Custom byte code

## memory types

*   stack segment
    *   one per coroutine
    *   contains stack frames, very much like the C stack
    *   is raw memory, as in C, i.e. no explicit type information
    *   stack grows upwards
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

Machine instruction operands are pushed left to right. They are marked `[stack-operand]`, while parameters in the code use `<opcode-operand>`

*   `exit <status>`
*   jumps; choose bit patterns such that unified handling is possible
    *   `absolute_jump <address>`
    *   `relative_jump <offset>`
    *   `relative_jump_if_true <offset>`
    *   `relative_jump_if_false <offset>`
    *   `indirect_jump [address]` (useful for tail recursion optimization of function pointers)
    *   indexed jumps could be added later
    *   `call <address>`
    *   `indirect_call [address]` (function pointers)
    *   `return`
*   one each for each size of integer and float:
    *   `push <value>`
    *   `add [op1] [op2]`
    *   `subtract [op1] [op2]`
    *   `multiply [op1] [op2]`
    *   `divide [op1] [op2]`
    *   `equals [op1] [op2]`
    *   `not_equals [op1] [op2]`
    *   `less_than [op1] [op2]`
    *   `less_than_or_equals [op1] [op2]`
    *   `greater_than [op1] [op2]`
    *   `greater_than_or_equals [op1] [op2]`
    *   `invert [operand]`
*   one for each size of integer:
    *   `modulo [op1] [op2]`
*   bitwise operators (including shifts) could conceivably be added later but would only make sense on unsigned types
*   boolean:
    *   `and [op1] [op2]`
    *   `or [op1] [op2]`
    *   `equals [op1] [op2]`
    *   `not_equals [op1] [op2]`
    *   `negate [operand]`
*   one for each size of data (`8|16|32|64`)
    *   (`load_heap [address]`: load value of that size from heap.) - there is no heap yet
    *   `absolute_load_stack [address]`
    *   `relative_load_stack <offset>`
    *   `absolute_store_stack [value] [address]`
    *   `relative_store_stack <offset> [value]`
*   `relative_load_stack <size> <offset>`
*   `absolute_load_stack <size> [address]`
*   (`load_heap <size> [address]`) - no heap yet
*   `absolute_store_stack <size> [value] [address]`
*   `relative_store_stack <size> <offset> [value]`
*   (`store_heap <size> [value] [address]`)
*   `pop <size>`
*   `syscall <index>`: a table of syscalls is created during compilation, so they are by index
*   there are explicitly _no type casts between signed and unsigned and between types of varying size_. These are prone to overflows, underflows and other potentially undefined or weird behaviour. If the user really needs them, he can provide a conversion function via the FFI. (This may turn out to be a horrible idea, in which case it will be revisited.)

## functions

Arguments are evaluated and pushed left to right. Callee removes them from the stack, enabling potential tail recursion optimization.