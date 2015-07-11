#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <utility>

#include <boost/variant/recursive_variant.hpp>
#include <boost/optional/optional.hpp>

#include "iterators.hpp"
#include "token_definitions.hpp"

// TODO: separate these into multiple files so you can only import ast::parser or ast::clean on demand?
namespace perseus
{
  namespace detail
  {
    namespace ast
    {
      struct string_literal : std::u32string, file_position
      {
        string_literal() = default;
        string_literal( const file_position& position  )
          : file_position( position )
        {
        }
      };

      struct void_expression
      {
      };

      struct identifier : std::string, file_position
      {
        identifier() = default;
        identifier( const enhanced_istream_iterator& begin, const enhanced_istream_iterator& end )
          : std::string( begin, end ), file_position( begin.get_position() )
        {
        }
      };

      struct operator_identifier : std::string, file_position
      {
        operator_identifier() = default;
        operator_identifier( const enhanced_istream_iterator& begin, const enhanced_istream_iterator& end )
          : std::string( begin, end ), file_position( begin.get_position() )
        {
        }
      };
      
      /// AST representation only used by the parser that is necessary to avoid left recursion but will be eliminated before further manipulation
      namespace parser
      {
#define PERSEUS_AST_PARSER
#include "ast_common_fwd.inl"

        struct binary_operation;
        struct call_expression;
        struct index_expression;
        
        typedef boost::variant<
          void_expression,
          string_literal,
          std::int32_t,
          bool,
          identifier,
          boost::recursive_wrapper< explicit_variable_declaration >,
          boost::recursive_wrapper< deduced_variable_declaration >,
          boost::recursive_wrapper< unary_operation >,
          boost::recursive_wrapper< if_expression >,
          boost::recursive_wrapper< while_expression >,
          boost::recursive_wrapper< return_expression >,
          boost::recursive_wrapper< block_expression >,
          boost::recursive_wrapper< parens_expression >
        > operand;

        typedef boost::variant<
          boost::recursive_wrapper< index_expression >,
          boost::recursive_wrapper< binary_operation >,
          boost::recursive_wrapper< call_expression >
        > operation;

        struct expression
        {
          operand head;
          std::vector< operation > tail;
        };

        struct binary_operation
        {
          std::string operation;
          operand operand;
        };

        struct call_expression
        {
          std::vector< expression > arguments;
        };

        struct index_expression
        {
          index_expression() = default;
          index_expression( const expression& exp )
            : index( exp )
          {
          }
          operator const expression&( ) const
          {
            return index;
          }
          expression index;
        };

#include "ast_common.inl"
#undef PERSEUS_AST_PARSER
      } // namespace parser

      namespace clean
      {
#define PERSEUS_AST_CLEAN
#include "ast_common_fwd.inl"

        struct binary_operation;
        struct call_expression;
        struct index_expression;

        typedef boost::variant<
          void_expression,
          string_literal,
          std::int32_t,
          bool,
          identifier,
          boost::recursive_wrapper< explicit_variable_declaration >,
          boost::recursive_wrapper< deduced_variable_declaration >,
          boost::recursive_wrapper< unary_operation >,
          boost::recursive_wrapper< if_expression >,
          boost::recursive_wrapper< while_expression >,
          boost::recursive_wrapper< return_expression >,
          boost::recursive_wrapper< block_expression >,
          boost::recursive_wrapper< parens_expression >,
          boost::recursive_wrapper< index_expression >,
          boost::recursive_wrapper< binary_operation >,
          boost::recursive_wrapper< call_expression >
        > expression;

        struct binary_operation
        {
          expression left_operand;
          std::string operation;
          expression right_operand;
        };

        struct call_expression
        {
          expression function;
          std::vector< expression > arguments;
        };

        struct index_expression
        {
          expression indexed;
          expression index;
        };

#include "ast_common.inl"
#undef PERSEUS_AST_CLEAN
      } // namespace clean
    } // namespace ast
  } //namespace detail
} // namespace perseus

