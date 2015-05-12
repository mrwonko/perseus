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
  struct invalid_opcode : std::invalid_argument
  {
    using std::invalid_argument::invalid_argument;
  };

  /**
  @brief Used exit opcode in a coroutine.
  */
  struct exit_in_coroutine : std::logic_error
  {
    using std::logic_error::logic_error;
  };

  /**
  @brief Pop from an empty stack

  Tried to pop more from a stack than is on it.
  */
  struct stack_underflow : std::out_of_range
  {
    using std::out_of_range::out_of_range;
  };

  /**
  @brief Supplied a @ref detail::coroutine::identifier "coroutine identifier" with no corresponding coroutine
  */
  struct invalid_coroutine_identifer : std::out_of_range
  {
    using std::out_of_range::out_of_range;
  };

  /**
  @brief Used @ref detail::opcode::delete_coroutine "delete_coroutine" on a live coroutine
  */
  struct deleting_live_coroutine : std::invalid_argument
  {
    using std::invalid_argument::invalid_argument;
  };

  /**
  @brief Tried to have more coroutines simultaneously than fit into @ref detail::coroutine::identifier
  @note Realistically this will never happen during normal usage
  */
  struct too_many_coroutines : std::logic_error
  {
    using std::logic_error::logic_error;
  };
}