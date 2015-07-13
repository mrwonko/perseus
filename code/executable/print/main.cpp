#include <string>
#include <iostream>
#include <fstream>
#include <utility>
#include <cctype>

#include <boost/spirit/home/qi/parse.hpp>
#include <boost/spirit/home/qi.hpp>

#include "compiler/compiler.hpp"
#include "compiler/steps.hpp"
#include "compiler/exceptions.hpp"
#include "compiler/iterators.hpp"
#include "compiler/token_definitions.hpp"
#include "compiler/grammar.hpp"
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

  // open input stream
  std::stringstream stringsource;
  std::ifstream filesource;
  std::string filename;
  if( argv[ 1 ] == "-e"s )
  {
    stringsource = std::stringstream( argv[ 2 ] );
    filename = "<string>";
  }
  else
  {
    filesource = std::ifstream( argv[ 1 ] );
    filename = argv[ 1 ];
  }
  std::istream& source = argv[ 1 ] == "-e"s ? static_cast< std::istream&>( stringsource ) : filesource;

  try
  {
    perseus::detail::enhanced_istream_iterator input_it, input_end;
    perseus::detail::skip_byte_order_mark( input_it, input_end );
    perseus::detail::token_definitions lexer;
    perseus::detail::token_iterator tokens_begin = lexer.begin( input_it, input_end );
    const perseus::detail::token_iterator tokens_end = lexer.end();

    // print tokens
    std::cout << "Tokens:" << std::endl;
    for( perseus::detail::token_iterator tokens_it = tokens_begin; tokens_it != tokens_end; ++tokens_it )
    {
      auto& token = *tokens_it;
      std::cout << " " << token.id() << " - \"";
      for( char c : std::string( token.value().begin(), token.value().end() ) )
      {
        if( c == '\n' )
          std::cout << "\\n";
        else if( c == '\t' )
          std::cout << "\\t";
        else if( c == '\\' )
          std::cout << "\\\\";
        else if( c == '"' )
          std::cout << "\\\"";
        else if( std::isprint( c ) )
          std::cout << c;
        else
          std::cout << "\\x" << std::hex << static_cast< unsigned int >( c );
      }
      std::cout << "\"" << std::endl;
    }

    // parse stream
    perseus::detail::grammar grammar;
    perseus::detail::skip_grammar skipper;
    perseus::compiler compiler;
    perseus::detail::ast::parser::file parser_ast;
    bool success = boost::spirit::qi::phrase_parse( tokens_begin, tokens_end, grammar, skipper, parser_ast );
    if( !success )
    {
      std::cerr << "phrase_parse() returned false." << std::endl;
      return 1;
    }
    if( tokens_begin != tokens_end )
    {
      std::cerr << "phrase_parse() did not consume all tokens." << std::endl;
      return 1;
    }
    parser::print_visitor{}( parser_ast );
    std::cout << "\n# Extracting functions..." << std::endl;
    auto functions = perseus::detail::extract_functions( parser_ast );
    std::cout << "# Checking types and references..." << std::endl;
    auto clean_ast = perseus::detail::clean_parser_ast( std::move( parser_ast ), functions );
    std::cout << "# Cleanded version:\n" << std::endl;
    clean::print_visitor{}( clean_ast );
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
      std::cerr << "Expectation failure: " << e.first->id() << " at " << e.first->value().begin().get_position() << std::endl;
    }
  }
  catch( perseus::compile_error& e )
  {
    std::cerr << "Compile error at " << e.location << ": " << e.what() << std::endl;
  }
  catch( std::exception& e )
  {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
  return 0;
}
