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

    void operator()( const ast::index_expression& exp ) const
    {
      indent() << "<index>" << std::endl;
      recurse( exp.index );
    }

    void operator()( const ast::binary_operation& exp ) const
    {
      indent() << "<binary " << static_cast< const std::string& >( exp.operation ) << ">" << std::endl;
      recurse_visit( exp.operand );
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
      print_visitor{ _indent + INDENT_SPACES }( x );
    }
    template< typename T >
    void recurse_visit( const T& x ) const
    {
      boost::apply_visitor( print_visitor{ _indent + INDENT_SPACES }, x );
    }
  };
}
