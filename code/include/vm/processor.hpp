#pragma once

#include <vector>
#include <functional>

#include "code_segment.hpp"
#include "instruction_pointer.hpp"
#include "stack.hpp"
#include "coroutine_manager.hpp"

namespace perseus
{
  namespace detail
  {
    /**
    @brief Syscall callback

    Invoked by the @ref opcode::syscall "syscall opcode"
    */
    typedef std::function< void( stack& ) > syscall;

    /**
    @brief Processor for Perseus bytecode.

    Responsible for execution of bytecode from a given @ref code_segment.

    @todo Support custom allocators? (Remove std::bad_alloc throw declarations then)
    */
    class processor
    {
    public:
      /**
      @brief Constructor.

      @param code Code this processor will execute. The processor takes ownership of it.
      @param syscalls Vector of syscalls for the @ref opcode::syscall "syscall opcode"
      */
      processor( code_segment&& code, std::vector< syscall >&& syscalls = {} );
      /// Non-copyable
      processor( const processor& ) = delete;
      /// Non-copyable
      processor& operator=( const processor& ) = delete;
      /// Move constructor
      processor( processor&& ) = default;
      /// Move assignment
      processor& operator=( processor&& ) = default;

      /**
      @brief Start execution at the given location.

      @param start_address Location in the code to begin execution at
      @param parameters Initial stack
      @returns Final stack
      @throws code_segmentation_fault if the instruction pointer reaches the end of the @ref code_segment.
      @throws invalid_opcode if the instruction pointer points to an invalid instruction.
      @throws too_many_coroutines if we run out of coroutine identifiers (basically only possible on incorrect usage)
      @throws invalid_coroutine_identifier if an invalid coroutine identifier is supplied
      @throws resuming_dead_coroutine when trying to resume a dead coroutine
      @throws resuming_live_coroutine when trying to resume an already live coroutine
      @throws bad_alloc if the system runs out of memory
      **/
      stack execute( const instruction_pointer::value_type start_address = 0, stack&& parameters = stack() );

      /**
      @brief Whether there are any coroutines
      */
      bool has_coroutines() const
      {
        return !_coroutine_manager.empty();
      }
    private:
      //    Opcodes

    private:
      //    Members

      /// Memory segment containing the code to be executed.
      code_segment _code;
      coroutine_manager _coroutine_manager;
      std::vector< syscall > _syscalls;
    };
  }
}
