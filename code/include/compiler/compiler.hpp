#pragma once

#include "compiler/ast.hpp"

#include <istream>
#include <memory>

namespace perseus
{
  /// The compiler takes care of turning source files into @ref detail::code_segment "byte code".
  class compiler
  {
  public:
    /// constructor
    compiler();
    /// destructor
    ~compiler();
    /// no copying; I see no need for it, and it may be expensive due to FST and parser copies.
    compiler( const compiler& ) = delete;
    /// dito - no copying
    compiler& operator=( const compiler& ) = delete;
    /// move constructor
    compiler( compiler&& );
    /// move assignment
    compiler& operator=( compiler&& );

    /**
    Parse the given Input Stream into a Syntax Tree, or throw an exception on failure.
    @param source_stream source code to parse
    @param filename name of the source file, for errors
    @return the syntax tree representation of the source code
    @todo document the various exceptions thrown on parse failure
    */
    detail::ast::parser::root parse( std::istream& source_stream, const std::string& filename ) const;
  private:
    struct impl;
    std::unique_ptr< impl > _impl;
  };
}
