tradeoffs to write about:

*   coroutines must be deleted before exit
    *   see perseus::undeleted_coroutine
*   stack contains no type information
*   stack vs registers
*   target platforms
*   choice of libraries/tools
    *   C++14
    *   boost.spirit
    *   boost.test
    *   cmake