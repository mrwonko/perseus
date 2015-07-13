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

        case string:
          return os << "string literal";
        case decimal_integer:
          return os << "decimal integer literal";
        case hexadecimal_integer:
          return os << "hexadecimal integer literal";
        case binary_integer:
          return os << "binary integer literal";

        case identifier:
          return os << "identifier";
        case operator_identifier:
          return os << "operator identifier";

        case if_:
          return os << "\"if\"";
        case else_:
          return os << "\"else\"";
        case while_:
          return os << "\"while\"";
        case return_:
          return os << "\"return\"";
        case function_:
          return os << "\"function\"";
        case true_:
          return os << "\"true\"";
        case false_:
          return os << "\"false\"";
        case mutable_:
          return os << "\"mutable\"";
        case impure_:
          return os << "\"impure\"";

        case colon:
          return os << "\":\"";
        case semicolon:
          return os << "\";\"";
        case dot:
          return os << "\".\"";
        case comma:
          return os << "\",\"";
        case equals:
          return os << "\"=\"";
        case backtick:
          return os << "\"`\"";
        case arrow_right:
          return os << "\"->\"";

        case paren_open:
          return os << "\"(\"";
        case paren_close:
          return os << "\")\"";
        case  brace_open:
          return os << "\"{\"";
        case brace_close:
          return os << "\"}\"";
        case square_bracket_open:
          return os << "\"[\"";
        case square_bracket_close:
          return os << "\"]\"";

        case any:
          return os << "any";
        default:
          return os << "token_id(0x" << std::hex << static_cast< unsigned int >( token ) << ')';
        }
      }
    }
  }
}
