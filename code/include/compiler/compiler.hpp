#pragma once

#include <istream>
#include <memory>

#include "vm/processor.hpp"

namespace perseus
{
  namespace detail
  {
    class code_segment;
  }

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

    The resulting syntax tree is kept internally and used during linking.
    @param source_stream source code to parse
    @param filename name of the source file, for errors
    @todo document the various exceptions thrown on parse failure
    */
    void parse( std::istream& source_stream, const std::string& filename );

    void reset();

    /**
    Generates code for all the parsed files. Places code to call main() at address 0.
    */
    detail::processor link();
  private:
    struct impl;
    std::unique_ptr< impl > _impl;
  };
}
