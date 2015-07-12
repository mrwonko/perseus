#include "compiler/steps.hpp"

#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>

#include <utility>

namespace perseus
{
  namespace detail
  {
    namespace clean = perseus::detail::ast::clean;
    namespace parser = perseus::detail::ast::parser;
    namespace art = perseus::detail::ast;

    static clean::expression convert( parser::expression&& exp );

    struct convert_block_member
    {
      clean::block_member operator()( parser::expression& exp )
      {
        return convert( std::move( exp ) );
      }
      clean::block_member operator()( parser::explicit_variable_declaration& dec ) const
      {
        return clean::explicit_variable_declaration{ dec.mut, std::move( dec.variable ), std::move( dec.type ), convert( std::move( dec.initial_value ) ) };
      }
      clean::block_member operator()( parser::deduced_variable_declaration& dec ) const
      {
        return clean::deduced_variable_declaration{ dec.mut, std::move( dec.variable ), convert( std::move( dec.initial_value ) ) };
      }
    };

    static clean::block_member convert( parser::block_member&& member )
    {
      return boost::apply_visitor( convert_block_member{}, std::move( member ) );
    }

    struct convert_operand
    {
      /// for shared types: int, bool, identifier, void & string literal
      template< typename T >
      clean::expression operator()( T& t ) const
      {
        return std::move( t );
      }
      clean::expression operator()( parser::unary_operation& op ) const
      {
        return clean::unary_operation{ std::move( op.operation ), convert( std::move( op.operand ) ) };
      }
      clean::expression operator()( parser::if_expression& exp ) const
      {
        return clean::if_expression{ convert( std::move( exp.condition ) ), convert( std::move( exp.then_expression ) ), convert( std::move( exp.else_expression ) ) };
      }
      clean::expression operator()( parser::while_expression& exp ) const
      {
        return clean::while_expression{ convert( std::move( exp.condition ) ), convert( std::move( exp.body ) ) };
      }
      clean::expression operator()( parser::return_expression& exp ) const
      {
        return clean::return_expression{ convert( std::move( exp.value ) ) };
      }
      clean::expression operator()( parser::block_expression& block ) const
      {
        std::vector< clean::block_member > members;
        for( parser::block_member& member : block.body )
        {
          members.emplace_back( convert( std::move( member ) ) );
        }
        return clean::block_expression{ std::move( members ) };
      }
      clean::expression operator()( parser::expression& exp ) const
      {
        return convert( std::move( exp ) );
      }
    };

    static clean::expression convert( parser::operand&& op )
    {
      return boost::apply_visitor( convert_operand{}, std::move( op ) );
    }

    static clean::expression convert( parser::expression&& exp )
    {
      struct convert_operation
      {
        clean::expression operator()( parser::binary_operation& op ) const
        {
          return clean::binary_operation{ std::move( lhs ), std::move( op.operation ), convert( std::move( op.operand ) ) };
        }
        clean::expression operator()( parser::index_expression& exp ) const
        {
          return clean::index_expression{ std::move( lhs ), convert( std::move( exp.index ) ) };
        }
        clean::expression operator()( parser::call_expression& call ) const
        {
          std::vector< clean::expression > args;
          for( parser::expression& arg : call.arguments )
          {
            args.emplace_back( convert( std::move( arg ) ) );
          }
          return clean::call_expression{ std::move( lhs ), std::move( args ) };
        }

        clean::expression lhs;
      };

      // TODO FIXME: take operator precedence into account!
      clean::expression root = convert( std::move( exp.head ) );
      for( parser::operation& op : exp.tail )
      {
        root = boost::apply_visitor( convert_operation{ std::move( root ) }, std::move( op ) );
      }
      return root;
    }

    clean::file clean_parser_ast( parser::file&& file, const function_manager& functions )
    {
      ast::clean::file result;
      for( auto& function : file.functions )
      {
        std::vector< clean::function_argument > args;
        for( parser::function_argument& arg : function.arguments )
        {
          args.emplace_back( clean::function_argument{ std::move( arg.name ), std::move( arg.type ) } );
        }
        result.functions.emplace_back( clean::function_definition{ std::move( function.name ), std::move( args ), std::move( function.type ), convert( std::move( function.body ) ) } );
      }
      return result;
    }
  }
}