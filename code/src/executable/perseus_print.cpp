#include <string>
#include <iostream>
#include <fstream>

#include <boost/spirit/home/lex/tokenize_and_parse.hpp>
#include <boost/spirit/home/qi.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>

#include "compiler/ast.hpp"
#include "compiler/compiler.hpp"
#include "util/u32string_ostream.hpp"

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

class print_visitor : public boost::static_visitor<>
{
  static constexpr unsigned int INDENT_SPACES = 2;

public:
  print_visitor( unsigned int indent = 0 )
    : _indent( indent )
    , _indent_string( indent, ' ' )
  {
  }

  void operator()( const ast::void_expression& ) const
  {
    indent() << "<void expression>" << std::endl;
  }

  void operator()( const ast::string_literal& str ) const
  {
    indent() << "<string literal \"" << static_cast< const std::u32string& >( str ) << "\">" << std::endl;
  }

  void operator()( std::uint32_t n ) const
  {
    indent() << "<integer literal " << n << ">" << std::endl;
  }

  void operator()( const ast::identifier& id ) const
  {
    indent() << "<identifier " << static_cast< const std::string& >( id ) << ">" << std::endl;
  }

  void operator()( const ast::unary_operation& op ) const
  {
    indent() << "<unary " << static_cast< const std::string& >( op.operation ) << ">" << std::endl;
    recurse( op.operand );
  }

  void operator()( const ast::if_expression& exp ) const
  {
    indent() << "<if>" << std::endl;
    recurse( exp.condition );
    recurse( exp.then_expression );
    recurse( exp.else_expression );
  }

  void operator()( const ast::while_expression& exp ) const
  {
    indent() << "<while>" << std::endl;
    recurse( exp.condition );
    recurse( exp.body );
  }

  void operator()( const ast::block_expression& block ) const
  {
    indent() << "<block>" << std::endl;
    for( const ast::expression& exp : block.body )
    {
      recurse( exp );
    }
  }

  void operator()( const ast::parens_expression& par ) const
  {
    indent() << "<parens>" << std::endl;
    recurse( par.body );
  }

  void operator()( const ast::index_expression& exp ) const
  {
    indent() << "<index>" << std::endl;
    recurse( exp.index );
  }

  void operator()( const ast::binary_operation& exp ) const
  {
    indent() << "<binary " << static_cast< const std::string& >( exp.operation ) << ">" << std::endl;
    recurse( exp.operand );
  }

  void operator()( const ast::call_expression& exp ) const
  {
    indent() << "<call>" << std::endl;
    for( const ast::expression& arg : exp.arguments )
    {
      recurse( arg );
    }
  }

  void operator()( const ast::expression& exp ) const
  {
    indent() << "<expression>" << std::endl;
    recurse( exp.head );
    for( const ast::operation& op : exp.tail )
    {
      recurse( op );
    }
  }

private:

  void recurse( const ast::expression& exp ) const
  {
    print_visitor{ _indent + INDENT_SPACES }( exp );
  }
  template< typename T >
  void recurse( const T& x ) const
  {
    boost::apply_visitor( print_visitor{ _indent + INDENT_SPACES }, x );
  }

  std::ostream& indent() const
  {
    return std::cout << _indent_string << "* ";
  }

  unsigned int _indent;
  std::string _indent_string;
};

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
  bool success = true;
  try
  {
    perseus::compiler compiler;
    if( argv[ 1 ] == "-e"s )
    {
      result = compiler.parse( std::stringstream( argv[ 2 ] ), "<string>" );
    }
    else
    {
      result = compiler.parse( std::ifstream( argv[ 1 ] ), argv[ 1 ] );
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
  print_visitor{}( result );
  return 0;
}
