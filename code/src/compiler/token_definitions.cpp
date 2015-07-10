#include "compiler/token_definitions.hpp"

namespace perseus
{
  namespace detail
  {
    token_definitions::token_definitions()
    {
      self.add_pattern
        ( "HEX", "[0-9a-zA-Z]" )
        ;

      self.add
        ( R"(\s+)", token_id::whitespace )
        ( R"(\/\/[^\n]*|\/\*([^\*]|\*[^\/])*\*\/)", token_id::comment )

        // literals
        ( R"(\"([^\n\"\\]|\\[\"nrt\\])*\")", token_id::string )
        
        ( R"(\d('?\d+)*)", token_id::decimal_integer )
        ( R"(0[xX]{HEX}('?{HEX}+)*)", token_id::hexadecimal_integer )
        ( R"(0[bB][01]('?[01]+)*)", token_id::binary_integer )

        // these keywords must come before identifier or they'd be matched by that
        ( R"(if)", token_id::if_ )
        ( R"(else)", token_id::else_ )
        ( R"(while)", token_id::while_ )
        ( R"(return)", token_id::return_ )
        ( R"(true)", token_id::true_ )
        ( R"(false)", token_id::false_ )
        ( R"(mut)", token_id::mut_ )
        ( R"(impure)", token_id::impure_ )
        ( R"(let)", token_id::let_ )
        ( R"(function)", token_id::function_ )
        ( R"(\w+)", token_id::identifier ) // alphanumeric and underscore

        ( R"(:)", token_id::colon )
        ( R"(;)", token_id::semicolon )
        ( R"(\.)", token_id::dot )
        ( R"(,)", token_id::comma )
        ( R"(=)", token_id::equals ) // must come before operator_ definition, since it's also covered by that
        ( R"(->)", token_id::arrow_right )
        ( R"(`)", token_id::backtick )
        ( R"(\()", token_id::paren_open )
        ( R"(\))", token_id::paren_close )
        ( R"(\{)", token_id::brace_open )
        ( R"(\})", token_id::brace_close )
        ( R"(\[)", token_id::square_bracket_open )
        ( R"(\])", token_id::square_bracket_close )
        // + - * . / | \ ! # $ % & < = > ? @ ^ ~
        ( R"([\+\-\*\.\/\|\\\!#$%&<=>\?@\^~]+)", token_id::operator_identifier )
        ;
    }
  }
}
