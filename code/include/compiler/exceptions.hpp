#pragma once

#include <stdexcept>

#include "iterators.hpp"

namespace perseus
{
  struct compile_error : std::runtime_error
  {
    compile_error( const std::string& what, const detail::file_position& location )
      : std::runtime_error( what ), location( location )
    {
    }
    detail::file_position location;
  };

  struct syntax_error : compile_error
  {
    using compile_error::compile_error;
  };

  struct semantic_error : compile_error
  {
    using compile_error::compile_error;
  };

  /**
  @brief File that was to be compiled is not encoded correctly (stray utf-8 continuations)
  */
  struct invalid_utf8 : syntax_error
  {
    using syntax_error::syntax_error;
  };

  struct integer_literal_overflow : syntax_error
  {
    using syntax_error::syntax_error;
  };
}