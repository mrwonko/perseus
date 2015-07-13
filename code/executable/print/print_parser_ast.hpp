#pragma once

#include <boost/variant/apply_visitor.hpp>

#include <string>

#include "compiler/ast.hpp"

#include "print_ast.hpp"

namespace parser
{
  namespace ast
  {
    using namespace perseus::detail::ast;
    using namespace perseus::detail::ast::parser;
  }

  using namespace std::string_literals;


  class print_visitor : public ::print_visitor
  {
  public:
    using ::print_visitor::print_visitor;

#include "print_ast.inl"

    void operator()( const ast::function_definition& func )
    {
      indent()
        << "<"
        << ( func.pure ? "" : "impure " )
        << "function "
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

    void operator()( const ast::deduced_variable_declaration& dec ) const
    {
      indent() << "<deduced declaration of " << ( dec.mut ? "mutable " : "" ) << static_cast< const std::string& >( dec.variable ) << ">" << std::endl;
      recurse( dec.initial_value );
    }

    void operator()( const ast::explicit_variable_declaration& dec ) const
    {
      indent() << "<explicit declaration of " << ( dec.mut ? "mutable " : "" ) << static_cast< const std::string& >( dec.variable ) << ": " << static_cast< const std::string& >( dec.type ) << ">" << std::endl;
      recurse( dec.initial_value );
    }
    
    void operator()( const ast::identifier& id ) const
    {
      indent() << "<identifier " << static_cast< const std::string& >( id ) << ">" << std::endl;
    }

    void operator()( const ast::index_expression& exp ) const
    {
      indent() << "<index>" << std::endl;
      recurse( exp.index );
    }

    void operator()( const ast::unary_operation& op ) const
    {
      indent() << "<unary " << static_cast< const std::string& >( op.operation ) << ">" << std::endl;
      recurse( op.operand );
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

    void recurse( const ast::operand& x ) const
    {
      boost::apply_visitor( print_visitor{ _indent + INDENT_SPACES }, x );
    }
    void recurse( const ast::block_member& x ) const
    {
      boost::apply_visitor( print_visitor{ _indent + INDENT_SPACES }, x );
    }
    void recurse( const ast::operation& x ) const
    {
      boost::apply_visitor( print_visitor{ _indent + INDENT_SPACES }, x );
    }
    template< typename T >
    void recurse( const T& x ) const
    {
      print_visitor{ _indent + INDENT_SPACES }( x );
    }
  };
}
