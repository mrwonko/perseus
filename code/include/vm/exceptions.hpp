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
}