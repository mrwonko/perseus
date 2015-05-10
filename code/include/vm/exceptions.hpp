#pragma once

#include <stdexcept>

namespace perseus
{
  /**
  @brief Attempted to access data outside of a segment.
  */
  struct segmentation_fault : std::out_of_range
  {
    using std::out_of_range::out_of_range;
  };

  /**
  @brief Attempted @ref detail::instruction_pointer "instruction_pointer" read outside of @ref detail::code_segment "code_segment".
  */
  struct code_segmentation_fault : segmentation_fault
  {
    using segmentation_fault::segmentation_fault;
  };

  /**
  @brief Read an invalid opcode.
  
  Tried reading an @ref detail::opcode "opcode" from the @ref detail::code_segment "code_segment" but received an invalid one.
  */
  struct invalid_opcode : std::domain_error
  {
    using std::domain_error::domain_error;
  };

  /**
  @brief Pop from an empty stack

  Tried to pop more from a stack than is on it.
  */
  struct stack_underflow : std::out_of_range
  {
    using std::out_of_range::out_of_range;
  };
}