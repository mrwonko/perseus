#include "compiler/grammar.hpp"
#include "compiler/conversions.hpp"

#include <boost/spirit/home/qi.hpp>
#include <boost/spirit/home/lex.hpp>

using namespace std::string_literals;

namespace perseus
{
  namespace detail
  {
    // rule definition; optional attributes are generated from the matched code
    template< typename attribute = boost::spirit::unused_type >
    using rule = boost::spirit::qi::rule< token_iterator, attribute(), skip_grammar >;

    //    terminals

    static rule< ast::string_literal > string{ string_literal_parser{}, "string literal"s };
    static rule< std::int32_t > decimal_integer{ decimal_integer_literal_parser{}, "decimal integer"s };
    static rule< std::int32_t > hexadecimal_integer{ hexadecimal_integer_literal_parser{}, "hexadecimal integer"s };
    static rule< std::int32_t > binary_integer{ binary_integer_literal_parser{}, "binary integer"s };
    static rule< std::int32_t > integer{ decimal_integer | hexadecimal_integer | binary_integer, "integer"s };

    static rule< ast::identifier > identifier{ boost::spirit::qi::token( token_id::identifier ), "identifier"s };
    static rule< ast::operator_identifier > operator_identifier{ boost::spirit::qi::token( token_id::operator_identifier ), "operator identifier"s };

    static rule<> if_{ boost::spirit::qi::token( token_id::if_ ), "if"s };
    static rule<> else_{ boost::spirit::qi::token( token_id::else_ ), "else"s };
    static rule<> while_{ boost::spirit::qi::token( token_id::while_ ), "while"s };
    static rule<> return_{ boost::spirit::qi::token( token_id::if_ ), "return"s };

    static rule<> colon{ boost::spirit::qi::token( token_id::colon ), "colon"s };
    static rule<> semicolon{ boost::spirit::qi::token( token_id::semicolon ), "semicolon"s };
    static rule<> dot{ boost::spirit::qi::token( token_id::dot ), "dot"s };
    static rule<> comma{ boost::spirit::qi::token( token_id::comma ), "comma"s };
    static rule<> equals{ boost::spirit::qi::token( token_id::equals ), "equals sign"s };
    static rule<> backtick{ boost::spirit::qi::token( token_id::backtick ), "backtick"s };

    static rule<> paren_open{ boost::spirit::qi::token( token_id::paren_open ), "opening paren"s };
    static rule<> paren_close{ boost::spirit::qi::token( token_id::paren_close ), "closing paren"s };
    static rule<> brace_open{ boost::spirit::qi::token( token_id::brace_open ), "opening brace"s };
    static rule<> brace_close{ boost::spirit::qi::token( token_id::brace_close ), "closing brace"s };
    static rule<> square_bracket_open{ boost::spirit::qi::token( token_id::square_bracket_open ), "opening square bracket"s };
    static rule<> square_bracket_close{ boost::spirit::qi::token( token_id::square_bracket_close ), "closing square bracket"s };

    //    non-terminals
    static grammar::start_type file;

    static rule< ast::expression > expression{ "expression"s };
    static rule< ast::operand > operand{ "operand"s };
    static rule< ast::operation > operation{ "operation"s };
    static rule< ast::binary_operation > binary_operation{ "binary operation"s };
    static rule< ast::unary_operation > unary_operation{ "unary operation"s };
    static rule< ast::if_expression > if_expression{ "if expression"s };
    static rule< ast::while_expression > while_expression{ "while expression"s };
    static rule< ast::call_expression > call_expression{ "call expression"s };
    static rule< ast::block_expression > block_expression{ "block expression"s };
    static rule< ast::parens_expression > parens_expression{ "parens expression"s };
    static rule< ast::index_expression > index_expression{ "index expression"s };


    grammar::grammar()
      : base_type( file, "perseus script"s )
    {
      // EOI = End of Input
      file %= expression > boost::spirit::qi::eoi;

      // what about operator_identifier? first class functions and all that?
      // this split is required to prevent left recursion, which in the parser turns into an infinite recursion.
      expression = operand >> *operation;
      operand = string | integer | identifier | unary_operation | if_expression | while_expression | block_expression | parens_expression;
      operation = binary_operation | call_expression | index_expression;
      {
        // x `op` y
        binary_operation = operator_identifier >> expression;

        // name( arg1, arg2 )
        // a % b means list of a separated by b; that has a minimum length of 1, thus the - (optional)
        call_expression = paren_open > -( expression % comma ) > paren_close;
        
        // object[index]
        index_expression = square_bracket_open > expression > square_bracket_close;

        // `op` x
        unary_operation = operator_identifier > expression;

        // if cond then_body else_body
        // Logically there's always an else, but it may be "nothing" (i.e. void).
        // > is an expectation concatenation: after an "if" terminal there *must* be an expression (allows for early abortion in case of errors and better errors)
        // this parsing is eager, i.e. `if c1 if c2 t else e` means `if c1 { if c2 t else e }`
        if_expression = if_ > expression > expression > ( ( else_ > expression ) | boost::spirit::qi::attr( ast::expression{ ast::void_expression{}, {} } ) );

        // while cond body
        while_expression = while_ > expression > expression;

        // { exp1; exp2 }
        block_expression = brace_open > ( expression % semicolon ) > brace_close;

        // ( expression )
        parens_expression = paren_open > expression > paren_close;
      }
    }
  }
  namespace detail
  {
    using skip_rule = boost::spirit::qi::rule< token_iterator >;

    // terminals
    static skip_rule whitespace{ boost::spirit::qi::token( token_id::whitespace ), "whitespace"s };
    static skip_rule comment{ boost::spirit::qi::token( token_id::whitespace ), "comment"s };
    // start symbol
    static skip_grammar::start_type skip{ whitespace | comment };

    skip_grammar::skip_grammar()
      : base_type( skip )
    {
    }
  }
}
