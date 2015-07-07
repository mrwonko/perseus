#pragma once

#include <stdexcept>

#include "iterators.hpp"

namespace perseus
{
  /**
  @brief File that was to be compiled is not encoded correctly (stray utf-8 continuations)
  */
  struct invalid_file : std::runtime_error
  {
    invalid_file( const char* what, const detail::file_position& location )
      : std::runtime_error( what ), location( location )
    {
    }
    detail::file_position location;
  };

  struct invalid_utf8 : invalid_file
  {
    using invalid_file::invalid_file;
  };
}