# VM architecture

## Layers

*   host
*   host API
*   metadata: types, functions etc.
    *   required e.g. for persistence, which the code processor does not do itself
*   code processor

## Hierarchy

*   runtime environment
    *   heap
    *   coroutines
        *   stack
        *   instruction pointer
    *   stack of active coroutines

## Stack

*   One per Coroutine
*   growing up, i.e. starts at index 0

## References

*   Type
    *   `0`: heap
    *   `n`: stack of coroutine `n - 1`
*   Pointer
    *   heap: absolute address
    *   stack: offset into stack

## Heap

*   Allocations done as `byte[]`, any interpretation is up to the bytecode

## Code segment

*   One contiunous code segment; there is only static linking at startup (for the time being?)
    *   the same goes for syscalls; they are by index, which is determined before compiling
*   main function starts at address `0`
*   execution at arbitrary addresses possible; abstraction of what is where is a different layer
