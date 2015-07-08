#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <utility>

#include <boost/variant/recursive_variant.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include "iterators.hpp"
#include "token_definitions.hpp"

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

      struct binary_operation;
      struct unary_operation;
      struct if_expression;
      struct while_expression;
      struct call_expression;

      typedef boost::variant<
        void_expression,
        string_literal,
        std::int32_t,
        identifier,
        boost::recursive_wrapper< binary_operation >,
        boost::recursive_wrapper< unary_operation >,
        boost::recursive_wrapper< if_expression >,
        boost::recursive_wrapper< while_expression >,
        boost::recursive_wrapper< call_expression >
      > expression;

      struct binary_operation
      {
        std::string operation;
        expression left_operand, right_operand;
      };

      struct unary_operation
      {
        std::string operation;
        expression operand_;
      };

      struct if_expression
      {
        expression condition, then_expression, else_expression;
      };

      struct while_expression
      {
        expression condition, body;
      };

      struct call_expression
      {
        expression name;
        std::vector< expression > arguments;
      };
    }
  }
}

// these must be at global scope

BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::binary_operation, left_operand, operation, right_operand );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::unary_operation, operation, operand_ );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::if_expression, condition, then_expression, else_expression );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::while_expression, condition, body );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::call_expression, name, arguments );
