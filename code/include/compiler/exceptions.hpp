#pragma once

#include <stdexcept>

#include "iterators.hpp"

namespace perseus
{
  /**
  @brief File that was to be compiled is not encoded correctly (stray utf-8 continuations)
  */
  struct syntax_error : std::runtime_error
  {
    syntax_error( const std::string& what, const detail::file_position& location )
      : std::runtime_error( what ), location( location )
    {
    }
    detail::file_position location;
  };

  struct invalid_utf8 : syntax_error
  {
    using syntax_error::syntax_error;
  };

  struct integer_literal_overflow : syntax_error
  {
    using syntax_error::syntax_error;
  };
}