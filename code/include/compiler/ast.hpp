#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <utility>

#include <boost/variant/recursive_variant.hpp>
#include <boost/optional/optional.hpp>

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
      struct return_expression;
      struct call_expression;
      struct block_expression;
      struct parens_expression;
      struct index_expression;
      struct explicit_variable_declaration;
      struct deduced_variable_declaration;
      
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
        expression operand;
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

      struct unary_operation
      {
        std::string operation;
        expression operand;
      };

      struct if_expression
      {
        expression condition, then_expression, else_expression;
      };

      struct while_expression
      {
        expression condition, body;
      };

      struct return_expression
      {
        return_expression() = default;
        return_expression( const expression& exp )
          : value( exp )
        {
        }
        operator const expression&( ) const
        {
          return value;
        }
        expression value;
      };

      struct block_expression
      {
        std::vector< expression > body;
      };

      /**
      @brief Explicit precedence using parens

      Binary expressions in the ast will be re-ordered according to their associativity and precedence; we need to keep track of explicitly defined precedence so we don't reorder that.
      */
      struct parens_expression
      {
        parens_expression() = default;
        parens_expression( const expression& exp )
          : body( exp )
        {
        }
        operator const expression&() const
        {
          return body;
        }

        expression body;
      };

      struct explicit_variable_declaration
      {
        identifier variable;
        identifier type;
        expression initial_value;
      };

      struct deduced_variable_declaration
      {
        identifier variable;
        expression initial_value;
      };

      struct function_argument
      {
        identifier name;
        identifier type;
      };

      struct function_definition
      {
        identifier name;
        std::vector< function_argument > arguments;
        boost::optional< identifier > type;
        expression body;
      };

      struct file
      {
        std::vector< function_definition > functions;
      };

      typedef file root;
    }
  }
}

