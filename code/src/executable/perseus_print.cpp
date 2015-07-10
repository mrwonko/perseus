#include <string>
#include <iostream>
#include <fstream>
#include <typeinfo>

#include <boost/spirit/home/lex/tokenize_and_parse.hpp>
#include <boost/spirit/home/qi.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>

#include "compiler/ast.hpp"
#include "compiler/compiler.hpp"
#include "util/u32string_ostream.hpp"

using namespace std::string_literals;

void print_usage( const char* path )
{
  std::cout << "Usage: " << path << R"( [-h | -e <code> | <filename>]
 -h        : show this help
 -e <code> : parse the given string
 <filename>: parse the given file
Parses the given source and prints its syntax tree.)" << std::endl;
}

namespace
{
  namespace ast
  {
    using namespace perseus::detail::ast;
    using namespace perseus::detail::ast::parser;
  }


  class parser_print_visitor : public boost::static_visitor<>
  {
    static constexpr unsigned int INDENT_SPACES = 2;

  public:
    parser_print_visitor( unsigned int indent = 0 )
      : _indent( indent )
      , _indent_string( indent, ' ' )
    {
    }

    void operator()( const ast::file& file ) const
    {
      indent() << "<file>" << std::endl;
      for( const ast::function_definition& func : file.functions )
      {
        recurse( func );
      }
    }

    void operator()( const ast::function_definition& func )
    {
      indent()
        << "<function "
        << static_cast< const std::string& >( func.name )
        << " -> "
        << ( func.type ? static_cast< const std::string& >( *func.type ) : "()"s )
        << ">"
        << std::endl;
      for( const ast::function_argument& arg : func.arguments )
      {
        recurse( arg );
      }
      recurse( func.body );
    }

    void operator()( const ast::function_argument& arg )
    {
      indent()
        << "<function argument "
        << static_cast< const std::string& >( arg.name )
        << ": "
        << static_cast< const std::string& >( arg.type )
        << std::endl;
    }

    void operator()( const ast::void_expression& ) const
    {
      indent() << "<void expression>" << std::endl;
    }

    void operator()( const ast::string_literal& str ) const
    {
      indent() << "<string literal \"" << static_cast< const std::u32string& >( str ) << "\">" << std::endl;
    }

    void operator()( std::int32_t n ) const
    {
      indent() << "<integer literal " << n << ">" << std::endl;
    }

    void operator()( bool b ) const
    {
      indent() << "<bool literal " << std::boolalpha << b << ">" << std::endl;
    }

    void operator()( const ast::identifier& id ) const
    {
      indent() << "<identifier " << static_cast< const std::string& >( id ) << ">" << std::endl;
    }

    void operator()( const ast::deduced_variable_declaration& dec ) const
    {
      indent() << "<deduced declaration of " << static_cast< const std::string& >( dec.variable ) << ">" << std::endl;
      recurse( dec.initial_value );
    }

    void operator()( const ast::explicit_variable_declaration& dec ) const
    {
      indent() << "<explicit declaration of " << static_cast< const std::string& >( dec.variable ) << ": " << static_cast< const std::string& >( dec.type ) << ">" << std::endl;
      recurse( dec.initial_value );
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

    void operator()( const ast::return_expression& exp ) const
    {
      indent() << "<return>" << std::endl;
      recurse( exp.value );
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
      recurse_visit( exp.head );
      for( const ast::operation& op : exp.tail )
      {
        recurse_visit( op );
      }
    }

  private:

    template< typename T >
    void recurse( const T& x ) const
    {
      parser_print_visitor{ _indent + INDENT_SPACES }( x );
    }
    template< typename T >
    void recurse_visit( const T& x ) const
    {
      boost::apply_visitor( parser_print_visitor{ _indent + INDENT_SPACES }, x );
    }

    std::ostream& indent() const
    {
      return std::cout << _indent_string << "* ";
    }

    unsigned int _indent;
    std::string _indent_string;
  };
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

  perseus::detail::ast::parser::file result;
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
  parser_print_visitor{}( result );
  return 0;
}
