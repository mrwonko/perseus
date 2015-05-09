#pragma once

#include "code_segment.hpp"
#include "instruction_pointer.hpp"

namespace perseus
{
  namespace detail
  {
    /**
    @brief Processor for Perseus bytecode.

    Responsible for execution of bytecode from a given @ref code_segment.
    */
    class processor
    {
    public:
      /**
      @brief Constructor.

      @param code Code this processor will execute. The processor takes ownership of it.
      */
      processor( code_segment&& code );
      /// Non-copyable
      processor( const processor& ) = delete;
      /// Non-copyable
      processor& operator=( const processor& ) = delete;
      /// Move constructor
      processor( processor&& ) = default;
      /// Move assignment
      processor& operator=( processor&& ) = default;

      /**
      @brief Start execution at address 0 in the bytecode.

      @throws code_segmentation_fault if the instruction pointer reaches the end of the @ref code_segment.
      @throws invalid_opcode if the instruction pointer points to an invalid instruction.
      **/
      void execute();
    private:
      //    Opcodes

    private:
      //    Members

      /// Memory segment containing the code to be executed.
      code_segment _code;
      instruction_pointer _instruction_pointer;
    };
  }
}
