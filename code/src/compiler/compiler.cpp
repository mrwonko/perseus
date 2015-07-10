#include "compiler/compiler.hpp"

#include "compiler/grammar.hpp"
#include "compiler/exceptions.hpp"

#include <boost/spirit/home/qi/parse.hpp>

namespace perseus
{
  struct compiler::impl
  {
    detail::token_definitions lexer;
    detail::grammar parser;
    detail::skip_grammar skipper;
  };

  compiler::compiler()
    : _impl( std::make_unique< compiler::impl >() )
  {
  }
  // these are in the .cpp since they require with compiler::impl, which is only forward declared in the .hpp
  compiler::~compiler() = default;
  compiler::compiler( compiler&& ) = default;
  compiler& compiler::operator=( compiler&& ) = default;

  /**
  Parse the given Input Stream into a Syntax Tree
  */
  detail::ast::parser::root compiler::parse( std::istream& source_stream, const std::string& filename ) const
  {
    detail::enhanced_istream_iterator input_it, input_end;
    std::tie( input_it, input_end ) = detail::enhanced_iterators( source_stream );
    detail::skip_byte_order_mark( input_it, input_end );

    // I suppose unless I need tokens_it/tokens_end, I could just lex::tokenize_and_phrase_parse, but doing it manually gives me more options
    detail::token_iterator tokens_it = _impl->lexer.begin( input_it, input_end );
    const detail::token_iterator tokens_end = _impl->lexer.end();

    detail::ast::parser::root result;
    bool success = boost::spirit::qi::phrase_parse( tokens_it, tokens_end, _impl->parser, _impl->skipper, result );

    if( !success )
    {
      // TODO: throw better error
      throw syntax_error( 
        "Failed to parse " + filename, 
        tokens_it == tokens_end ? detail::file_position() : tokens_it->value().begin().get_position()
        );
    }
    // no need to check if it == end, since the grammar contains an eoi parser.
    return result;
  }
}
