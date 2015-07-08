#include "compiler/grammar.hpp"
#include "compiler/conversions.hpp"

#include <boost/spirit/home/qi.hpp>
#include <boost/spirit/home/lex.hpp>

namespace perseus
{
  namespace detail
  {
    skip_grammar::skip_grammar()
      : base_type( skip )
      , whitespace( boost::spirit::qi::token( token_id::whitespace ), "whitespace"s )
      , comment( boost::spirit::qi::token( token_id::whitespace ), "comment"s )
    {
      skip = whitespace | comment;
    }

    grammar::grammar()
      : base_type( file, "perseus script"s )
      // terminals - full definition
      , byte_order_mark( boost::spirit::qi::token( token_id::byte_order_mark ), "UTF-8 byte order mark"s )

      , string( string_literal_parser{}, "string literal"s )

      , decimal_integer( decimal_integer_literal_parser{}, "decimal integer"s )
      , hexadecimal_integer( hexadecimal_integer_literal_parser{}, "hexadecimal integer"s )
      , binary_integer( binary_integer_literal_parser{}, "binary integer"s )
      , integer( decimal_integer | hexadecimal_integer | binary_integer, "integer"s )

      , identifier( boost::spirit::qi::token( token_id::identifier ), "identifier"s )
      , operator_identifier( boost::spirit::qi::token( token_id::operator_identifier ), "operator identifier"s )

      , if_( boost::spirit::qi::token( token_id::if_ ), "if"s )
      , else_( boost::spirit::qi::token( token_id::else_ ), "else"s )
      , while_( boost::spirit::qi::token( token_id::while_ ), "while"s )
      , return_( boost::spirit::qi::token( token_id::if_ ), "return"s )

      , colon( boost::spirit::qi::token( token_id::colon ), "colon"s )
      , semicolon( boost::spirit::qi::token( token_id::semicolon ), "semicolon"s )
      , dot( boost::spirit::qi::token( token_id::dot ), "dot"s )
      , comma( boost::spirit::qi::token( token_id::comma ), "comma"s )
      , equals( boost::spirit::qi::token( token_id::equals ), "equals sign"s )
      , backtick( boost::spirit::qi::token( token_id::backtick ), "backtick"s )

      , paren_open( boost::spirit::qi::token( token_id::paren_open ), "opening paren"s )
      , paren_close( boost::spirit::qi::token( token_id::paren_close ), "closing paren"s )
      , brace_open( boost::spirit::qi::token( token_id::brace_open ), "opening brace"s )
      , brace_close( boost::spirit::qi::token( token_id::brace_close ), "closing brace"s )
      , square_bracket_open( boost::spirit::qi::token( token_id::square_bracket_open ), "opening square bracket"s )
      , square_bracket_close( boost::spirit::qi::token( token_id::square_bracket_close ), "closing square bracket"s )

      // non-terminals - names only, definitions follow
      , file( "file"s )
      , expression( "expression"s )
      , binary_operation( "binary operation"s )
      , unary_operation( "unary operation"s )
      , if_expression( "if expression"s )
      , while_expression( "while expression"s )
      , call_expression( "call expression"s )
      , block_expression( "block expression"s )
      , parens_expression( "parens expression"s )
      , index_expression( "index expression"s )
    {
      // EOI = End of Input
      file %= ( -byte_order_mark ) > expression > boost::spirit::qi::eoi;

      // what about operator_identifier? first class functions and all that?
      expression = string | integer | identifier | binary_operation | unary_operation | if_expression | while_expression | call_expression | block_expression | parens_expression;

      // expression alternatives
      {
        // x `op` y
        binary_operation = expression >> operator_identifier >> expression;

        // `op` x
        unary_operation = operator_identifier >> expression;

        // if cond then_body else_body
        // Logically there's always an else, but it may be "nothing" (i.e. void).
        // > is an expectation concatenation: after an "if" terminal there *must* be an expression (allows for early abortion in case of errors and better errors)
        // this parsing is eager, i.e. `if c1 if c2 t else e` means `if c1 { if c2 t else e }`
        if_expression = if_ > expression > ( ( else_ > expression ) | boost::spirit::qi::attr( ast::void_expression{} ) );

        // while cond body
        while_expression = while_ > expression > expression;

        // name( arg1, arg2 )
        // a % b means list of a separated by b
        call_expression = expression >> paren_open > ( expression % comma ) > paren_close;

        // { exp1; exp2 }
        block_expression = brace_open > ( expression % semicolon ) > brace_close;

        // ( expression )
        parens_expression = paren_open > expression > paren_close;

        // object[index]
        index_expression = expression >> square_bracket_open > expression > square_bracket_close;
      }
    }
  }
}
