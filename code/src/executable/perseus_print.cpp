#include <string>
#include <iostream>
#include <fstream>

#include <boost/spirit/home/lex/tokenize_and_parse.hpp>
#include <boost/spirit/home/qi.hpp>

#include "compiler/grammar.hpp"

using namespace std::string_literals;
namespace ast = perseus::detail::ast;

void print_usage( const char* path )
{
  std::cout << "Usage: " << path << R"( [-h | -e <code> | <filename>]
 -h        : show this help
 -e <code> : parse the given string
 <filename>: parse the given file
Parses the given source and prints its syntax tree.)" << std::endl;
}

bool parse( std::istream& source, ast::root& out_result )
{
  if( !source )
  {
    std::cerr << "Could not open file!" << std::endl;
    return false;
  }
  perseus::detail::enhanced_istream_iterator source_begin, source_end;
  std::tie( source_begin, source_end ) = perseus::detail::enhanced_iterators( source );

  return boost::spirit::lex::tokenize_and_phrase_parse( source_begin, source_end, perseus::detail::token_definitions{}, perseus::detail::grammar{}, perseus::detail::skip_grammar{}, out_result );
}

int main( const int argc, const char* argv[] )
{
  if( argc < 2
    || argv[ 1 ] == "-h"s || argv[ 1 ] == "--help"s
    || argv[ 1 ] != "-e"s && argc != 2
    || argv[ 1 ] == "-e"s && argc != 3
    )
  {
    print_usage( argv[ 0 ] );
    return 0;
  }

  ast::root result;
  bool success = false;
  try
  {
    if( argv[ 1 ] == "-e"s )
    {
      success = parse( std::stringstream( argv[ 2 ] ), result );
    }
    else
    {
      success = parse( std::ifstream( argv[ 1 ] ), result );
    }
  }
  catch( boost::spirit::qi::expectation_failure< perseus::detail::token_iterator >& e )
  {
    // FIXME: the dereferencing presumably fails for empty inputs
    std::cerr << "Unexpected token " << e.first->id() << " at " << e.first->value().begin().get_position() << std::endl;
    success = false;
  }
  catch( std::exception& e )
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    success = false;
  }
  if( !success )
  {
    std::cerr << "Could not parse file!" << std::endl;
    return 1;
  }
  // TODO
  return 0;
}
