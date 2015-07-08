#include "compiler/token_ids.hpp"

namespace perseus
{
  namespace detail
  {
    namespace token_id
    {
      std::ostream& operator<<( std::ostream& os, token_id token )
      {
        switch( token )
        {
        case whitespace:
          return os << "whitespace";
        case comment:
          return os << "comment";
        case byte_order_mark:
          return os << "byte_order_mark";

        case string:
          return os << "string";

        case identifier:
          return os << "identifier";
        case operator_identifier:
          return os << "operator identifier";

        case if_:
          return os << "if";
        case else_:
          return os << "else";
        case while_:
          return os << "while";
        case return_:
          return os << "return";

        case colon:
          return os << "colon";
        case semicolon:
          return os << "semicolon";
        case dot:
          return os << "dot";
        case comma:
          return os << "comma";
        case equals:
          return os << "equals sign";
        case backtick:
          return os << "backtick";

        case paren_open:
          return os << "opening paren";
        case paren_close:
          return os << "closing paren";
        case  brace_open:
          return os << "opening brace";
        case brace_close:
          return os << "closing brace";
        case square_bracket_open:
          return os << "opening square bracket";
        case square_bracket_close:
          return os << "closing square bracket";

        case any:
          return os << "any";
        default:
          return os << "token_id(" << static_cast< unsigned int >( token ) << ')';
        }
      }
    }
  }
}
