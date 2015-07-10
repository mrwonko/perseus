#include "compiler/grammar.hpp"
#include "compiler/conversions.hpp"
#include "compiler/ast_adapted.hpp"

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

    // literals
    static rule< ast::string_literal > string{ string_literal_parser{}, "string literal"s };
    static rule< std::int32_t > decimal_integer{ decimal_integer_literal_parser{}, "decimal integer"s };
    static rule< std::int32_t > hexadecimal_integer{ hexadecimal_integer_literal_parser{}, "hexadecimal integer"s };
    static rule< std::int32_t > binary_integer{ binary_integer_literal_parser{}, "binary integer"s };
    static rule< std::int32_t > integer{ decimal_integer | hexadecimal_integer | binary_integer, "integer"s };
    static rule< bool > true_{ boost::spirit::qi::omit[ boost::spirit::qi::token( token_id::true_ ) ] > boost::spirit::qi::attr( true ), "true"s };
    static rule< bool > false_{ boost::spirit::qi::omit[ boost::spirit::qi::token( token_id::false_ ) ] > boost::spirit::qi::attr( false ), "false"s };

    static rule< ast::identifier > identifier{ boost::spirit::qi::token( token_id::identifier ), "identifier"s };
    static rule< ast::operator_identifier > operator_identifier{ boost::spirit::qi::token( token_id::operator_identifier ), "operator identifier"s };

#define PERSEUS_TERMINAL( identifier, name ) static rule<> identifier{ boost::spirit::qi::token( token_id::identifier ), name }
    PERSEUS_TERMINAL( if_, "if"s );
    PERSEUS_TERMINAL( else_, "else"s );
    PERSEUS_TERMINAL( while_, "while"s );
    PERSEUS_TERMINAL( return_, "return"s );

    PERSEUS_TERMINAL( colon, "colon"s );
    PERSEUS_TERMINAL( semicolon, "semicolon"s );
    PERSEUS_TERMINAL( dot, "dot"s );
    PERSEUS_TERMINAL( comma, "comma"s );
    PERSEUS_TERMINAL( equals, "equals sign"s );
    PERSEUS_TERMINAL( backtick, "backtick"s );
    PERSEUS_TERMINAL( arrow_right, "arrow right"s );

    PERSEUS_TERMINAL( paren_open, "opening paren"s );
    PERSEUS_TERMINAL( paren_close, "closing paren"s );
    PERSEUS_TERMINAL( brace_open, "opening brace"s );
    PERSEUS_TERMINAL( brace_close, "closing brace"s );
    PERSEUS_TERMINAL( square_bracket_open, "opening square bracket"s );
    PERSEUS_TERMINAL( square_bracket_close, "closing square bracket"s );

    PERSEUS_TERMINAL( let_, "let" );
    PERSEUS_TERMINAL( function_, "function" );

    PERSEUS_TERMINAL( mut_, "mut" );
    PERSEUS_TERMINAL( impure_, "impure" );
#undef PERSEUS_TERMINAL
    
    //    non-terminals
    static grammar::start_type file;

    static rule< ast::expression > expression{ "expression"s };
    static rule< ast::operand > operand{ "operand"s };
    static rule< ast::operation > operation{ "operation"s };
    static rule< ast::binary_operation > binary_operation{ "binary operation"s };
    static rule< ast::unary_operation > unary_operation{ "unary operation"s };
    static rule< ast::if_expression > if_expression{ "if expression"s };
    static rule< ast::while_expression > while_expression{ "while expression"s };
    static rule< ast::return_expression > return_expression{ "return expression"s };
    static rule< ast::call_expression > call_expression{ "call expression"s };
    static rule< ast::block_expression > block_expression{ "block expression"s };
    static rule< ast::parens_expression > parens_expression{ "parens expression"s };
    static rule< ast::index_expression > index_expression{ "index expression"s };
    static rule< ast::explicit_variable_declaration > explicit_variable_declaration{ "explicit variable declaration"s };
    static rule< ast::deduced_variable_declaration > deduced_variable_declaration{ "deduced variable declaration"s };
    static rule< ast::function_definition > function_definition{ "function definition"s };
    static rule< ast::function_argument > function_argument{ "function argument"s };


    grammar::grammar()
      : base_type( file, "perseus script"s )
    {
      // EOI = End of Input
      file = +function_definition > boost::spirit::qi::eoi;

      function_definition = function_ > identifier > paren_open > -( function_argument % comma ) > paren_close > -( arrow_right > identifier ) > expression;
      {
        function_argument = identifier > colon > identifier;
        // this split is required to prevent left recursion, which in the parser turns into an infinite recursion.
        expression = operand >> *operation;
        {
          // what about operator_identifier? first class functions and all that?
          operand = string | integer | true_ | false_ | identifier | unary_operation | if_expression | while_expression | return_expression | block_expression | parens_expression | explicit_variable_declaration | deduced_variable_declaration;
          {
            // `op` x
            unary_operation = operator_identifier > expression;

            // if cond then_body else_body
            // Logically there's always an else, but it may be "nothing" (i.e. void).
            // > is an expectation concatenation: after an "if" terminal there *must* be an expression (allows for early abortion in case of errors and better errors)
            // this parsing is eager, i.e. `if c1 if c2 t else e` means `if c1 { if c2 t else e }`
            auto default_to_void = boost::spirit::qi::attr( ast::expression{ ast::void_expression{},{} } );
            if_expression = if_ > expression > expression > ( ( else_ > expression ) | default_to_void );

            // while cond body
            while_expression = while_ > expression > expression;

            // return exp
            return_expression = return_ > ( expression | default_to_void );

            // { exp1; exp2 }
            block_expression = brace_open > ( ( expression | default_to_void ) % semicolon ) > brace_close;

            // ( expression )
            parens_expression = paren_open > expression > paren_close;

            // let x : t = v
            explicit_variable_declaration = let_ >> identifier >> colon > identifier > equals > expression;
            // let x = v
            deduced_variable_declaration = let_ >> identifier >> equals > expression;
          }
          operation = binary_operation | call_expression | index_expression;
          {
            // x `op` y
            binary_operation = operator_identifier >> expression;

            // name( arg1, arg2 )
            // a % b means list of a separated by b; that has a minimum length of 1, thus the - (optional)
            call_expression = paren_open > -( expression % comma ) > paren_close;

            // object[index]
            index_expression = square_bracket_open > expression > square_bracket_close;
          }
        }
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
