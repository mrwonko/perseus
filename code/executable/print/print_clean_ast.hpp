#pragma once

#include <boost/variant/apply_visitor.hpp>

#include <string>

#include "compiler/ast.hpp"
#include "compiler/types.hpp"

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

    void operator()( const ast::function_definition& func )
    {
      indent()
        << "<function "
        << func.manager_entry->first.name
        << "(";
      bool first = true;
      for( perseus::detail::type_id type : func.manager_entry->first.parameters )
      {
        if( !first )
        {
          std::cout << ", ";
        }
        std::cout << perseus::detail::get_name( type );
        first = false;
      }
      std::cout << ")"
        << " -> "
        << perseus::detail::get_name( func.manager_entry->second.return_type )
        << ">"
        << std::endl;
      recurse( func.body );
    }

    void operator()( const ast::variable_declaration& dec ) const
    {
      indent() << "<variable declaration>" << std::endl;
      recurse( dec.initial_value );
    }

    void operator()( const ast::local_variable_reference& dec ) const
    {
      indent() << "<variable reference at offset " << dec.offset << ">" << std::endl;
    }

    void operator()( const ast::call_expression& exp ) const
    {
      indent()
        << "<call "
        << exp.function->first.name
        << ">" << std::endl;
      for( const ast::expression& arg : exp.arguments )
      {
        recurse( arg );
      }
    }

    // called by block_member visitation
    void operator()( const ast::expression& exp ) const
    {
      indent() << "<" << get_name( exp.type ) << " expression>" << std::endl;
      boost::apply_visitor( print_visitor{ _indent + 2 * INDENT_SPACES }, exp.subexpression );
    }

  private:

    void recurse( const ast::expression& exp ) const
    {
      print_visitor{ _indent + INDENT_SPACES }( exp );
    }
    void recurse( const ast::block_member& member ) const
    {
      boost::apply_visitor( print_visitor{ _indent + INDENT_SPACES }, member );
    }
    template< typename T >
    void recurse( const T& x ) const
    {
      print_visitor{ _indent + INDENT_SPACES }( x );
    }
  };
}
