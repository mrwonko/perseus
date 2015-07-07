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
          return os << "string";
        case identifier:
          return os << "identifier";
        case any:
          return os << "any";
        case byte_order_mark:
          return os << "byte_order_mark";
        default:
          return os << "token_id(" << static_cast< unsigned int >( token ) << ')';
        }
      }
    }
  }
}
