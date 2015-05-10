#pragma once

#include <vector>

#include "code_segment.hpp"
#include "instruction_pointer.hpp"
#include "stack.hpp"
#include "coroutine_manager.hpp"

namespace perseus
{
  namespace detail
  {
    /**
    @brief Processor for Perseus bytecode.

    Responsible for execution of bytecode from a given @ref code_segment.

    @todo Support custom allocators? (Remove std::bad_alloc throw declarations then)
    */
    class processor
    {
    public:
      /**
      @brief Result of @ref processor::continue_execution()
      */
      struct execution_result
      {
        /**
        @brief Whether it's illegal to call @ref processor::continue_execution() again.

        When the root coroutine yields, this is false. When the program exits, it's true.
        */
        bool execution_finished;
        /**
        @brief Returned (in case of opcode::exit) or yielded (in case of opcode::yield) result: a number of bytes off the top of the root coroutine's stack (possibly none).
        */
        stack stack;
      };

    public:
      /**
      @brief Constructor.

      @param code Code this processor will execute. The processor takes ownership of it.
      @param start_address The address to begin execution at.
      */
      processor( code_segment&& code, const instruction_pointer::value_type start_address = 0 );
      /// Non-copyable
      processor( const processor& ) = delete;
      /// Non-copyable
      processor& operator=( const processor& ) = delete;
      /// Move constructor
      processor( processor&& ) = default;
      /// Move assignment
      processor& operator=( processor&& ) = default;

      /**
      @brief Continue execution where it left off.

      The first time this is called, execution will start at address 0.

      If the root coroutine yields, the 

      @param parameters the current coroutine's stack
      @throws code_segmentation_fault if the instruction pointer reaches the end of the @ref code_segment.
      @throws invalid_opcode if the instruction pointer points to an invalid instruction.
      @throws bad_alloc if the system runs out of memory
      **/
      execution_result continue_execution( stack&& parameters = stack() );
    private:
      //    Opcodes

    private:
      //    Members

      /// Memory segment containing the code to be executed.
      code_segment _code;
      coroutine_manager _coroutine_manager;
      std::vector< coroutine* > _active_coroutine_stack;
    };
  }
}
