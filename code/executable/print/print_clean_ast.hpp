#pragma once

#include <boost/variant/apply_visitor.hpp>

#include <string>

#include "compiler/ast.hpp"

#include "print_ast.hpp"

namespace clean
{
  namespace ast
  {
    using namespace perseus::detail::ast;
    using namespace perseus::detail::ast::clean;
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
      recurse( exp.indexed );
      recurse( exp.index );
    }

    void operator()( const ast::binary_operation& exp ) const
    {
      indent() << "<binary " << static_cast< const std::string& >( exp.operation ) << ">" << std::endl;
      recurse( exp.right_operand );
      recurse( exp.left_operand );
    }

    void operator()( const ast::call_expression& exp ) const
    {
      indent() << "<call>" << std::endl;
      recurse( exp.function );
      for( const ast::expression& arg : exp.arguments )
      {
        recurse( arg );
      }
    }

  private:

    void recurse( const ast::expression& exp ) const
    {
      boost::apply_visitor( print_visitor{ _indent + INDENT_SPACES }, exp );
    }
    template< typename T >
    void recurse( const T& x ) const
    {
      print_visitor{ _indent + INDENT_SPACES }( x );
    }
  };
}
