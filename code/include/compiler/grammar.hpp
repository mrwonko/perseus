#pragma once

#include <boost/spirit/home/qi/nonterminal/rule.hpp>
#include <boost/spirit/home/qi/nonterminal/grammar.hpp>

#include "ast.hpp"
#include "token_definitions.hpp"

namespace perseus
{
  namespace detail
  {
    using namespace std::string_literals; // gain access to operator ""s

    /// Tokens to skip during parse; takes care of ignoring comments and whitespace
    class skip_grammar : public boost::spirit::qi::grammar< token_iterator >
    {
    public:
      /// constructor defining the grammar
      skip_grammar();

    private:
      using rule = boost::spirit::qi::rule< token_iterator >;

      // terminals
      rule whitespace;
      rule comment;
      // start symbol
      start_type skip;
    };

    /**
    @brief Syntax Definition of the Perseus Language
    */
    class grammar : public boost::spirit::qi::grammar< token_iterator, ast::expression(), skip_grammar >
    {
    public:
      /// constructor defining the grammar
      grammar();

    private:
      // rule definition; optional attributes are generated from the matched code
      template< typename attribute = boost::spirit::unused_type() >
      using rule = boost::spirit::qi::rule< token_iterator, attribute, skip_grammar >;

      //    terminals

      rule<> byte_order_mark;
      rule< ast::string_literal() > string;
      rule< std::int32_t() > decimal_integer;
      rule< std::int32_t() > hexadecimal_integer;
      rule< std::int32_t() > binary_integer;
      rule< std::int32_t() > integer;

      rule< ast::identifier() > identifier;
      rule< ast::operator_identifier() > operator_identifier;

      rule<> if_;
      rule<> else_;
      rule<> while_;
      rule<> return_;

      rule<> colon;
      rule<> semicolon;
      rule<> dot;
      rule<> comma;
      rule<> equals;
      rule<> backtick;

      rule<> paren_open;
      rule<> paren_close;
      rule<> brace_open;
      rule<> brace_close;
      rule<> square_bracket_open;
      rule<> square_bracket_close;

      //    non-terminals
      start_type file;

      rule< ast::expression() > expression;
      rule< ast::binary_operation() > binary_operation;
      rule< ast::unary_operation() > unary_operation;
      rule< ast::if_expression() > if_expression;
      rule< ast::while_expression() > while_expression;
      rule< ast::call_expression() > call_expression;
      rule< ast::block_expression() > block_expression;
      rule< ast::parens_expression() > parens_expression;
      rule< ast::index_expression() > index_expression;
    };
  }
}
