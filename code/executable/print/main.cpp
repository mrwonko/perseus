#include <string>
#include <iostream>
#include <fstream>
#include <utility>

#include <boost/spirit/home/lex/tokenize_and_parse.hpp>
#include <boost/spirit/home/qi.hpp>

#include "compiler/compiler.hpp"
#include "compiler/steps.hpp"
#include "util/u32string_ostream.hpp"

#include "print_parser_ast.hpp"
#include "print_clean_ast.hpp"

using namespace std::string_literals;

void print_usage( const char* path )
{
  std::cout << "Usage: " << path << R"( [-h | -e <code> | <filename>]
 -h        : show this help
 -e <code> : parse the given string
 <filename>: parse the given file
Parses the given source and prints its syntax tree.)" << std::endl;
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

  perseus::detail::ast::parser::file parser_ast;
  bool success = true;
  try
  {
    perseus::compiler compiler;
    if( argv[ 1 ] == "-e"s )
    {
      parser_ast = compiler.parse( std::stringstream( argv[ 2 ] ), "<string>" );
    }
    else
    {
      parser_ast = compiler.parse( std::ifstream( argv[ 1 ] ), argv[ 1 ] );
    }
  }
  catch( boost::spirit::qi::expectation_failure< perseus::detail::token_iterator >& e )
  {
    // FIXME: the dereferencing presumably fails for empty inputs
    if( e.first == perseus::detail::token_definitions{}.end() )
    {
      std::cerr << "Unexpected End of File" << std::endl;
    }
    else
    {
      std::cerr << "Unexpected token " << e.first->id() << " at " << e.first->value().begin().get_position() << std::endl;
    }
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
  parser::print_visitor{}( parser_ast );
  auto clean_ast = perseus::detail::clean_parser_ast( std::move( parser_ast ) );
  std::cout << "\n# Cleanded version:\n" << std::endl;
  clean::print_visitor{}( clean_ast );
  return 0;
}
