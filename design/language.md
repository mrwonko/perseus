# Perseus Language

## Core

Anything that needs to be added ASAP.

*   C-like syntax?
*   const/mutable
    *   const by default
*   pure/impure
    *   pure by default
    *   are memory allocations pure? presumably not, since they can fail depending on external state.
*   serializable/non-serializable
*   Static Typing
*   Type inferring
*   Types:
    *   bool
    *   char32 (UTF32 for simplicity/consistency; memory is cheap?)
    *   int64 (actual size could be changeable at compile-time)
    *   float32
    *   array (static/dynamic?)
        *   dynamic array would imply both a Heap and pointers
        *   static arrays would suggest pointers if valid as parameters
    *   pointer (smart?), reference?
    *   record
*   Everything is an expression, as in Rust
    *   i.e. a sum function can be written like `fn sum( int a, int b ) -> int { a + b }`
    *   a trailing semicolon creates an empty expression, evaluating to void
    *   so since if/else is always an expression, it can be used like the ternary operator in C, given the types match: `x = if( cond ) 3 else 4;`
*   if & while statement
*   Procedures
    *   self-defined
    *   host-defined
    *   variable number of return values
        *   implies some form of tuples?
*   persistence
    *   I basically want to be able to save a suspended coroutine to disk and resume it later on a different machine. Details to be determined.

## Extension

The rest.

*   8/16/32 bit integers
*   unsigned integers
*   float64
*   for, do while, generic for
    *   give the for some more thought - 
*   Modules
    *   Ability to load modules at runtime
        *   To guarantee type safety, those need to adhere to a well-defined interface, so some kind of module interface definition is required
*   local procedures (should require no new opcodes?)
    *   closures
        *   Lifetime? Order of destruction? May need some fine control over what is and is not part of the closure?
            *   Not really an issue until objects are added?
*   defer Keyword
    *   See Go - defer execution of a statement to end of scope in reverse order, akin to destructors of local objects.
*   classes
    *   using something similar to Haskell's typeclasses
    *   definitely need a Destructor
    *   move semantics?
        *   move-only implies inability to serialize/persist
    *   inheritance?
*   exceptions?
    *   throw/catch with arbitrary types seems counter to static typing
    *   maybe Monads? with syntactic sugar akin to Haskell's do?
    *   might be core, they'd be useful with memory allocations
*   compile-time computations, e.g. for static array sizes (see C++'s constexpr)
    *   could be quite simple - any pure function can be evaluated at compile time, since the runtime environment is available anyway?
*   coroutines
    *   do they need closures? probably at least pointers?
*   templates/parametrized types
*   custom operators? (see Haskell)

