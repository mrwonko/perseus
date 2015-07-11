#include "compiler/types.hpp"

#include <map>

namespace perseus
{
  namespace detail
  {
    std::uint32_t get_size( type_id t )
    {
      switch( t )
      {
      case type_id::void_:
        return 0;
      case type_id::bool_:
        return 1;
      case type_id::i32:
        return 4;
      }
    }

    bool get_type( const std::string& name, type_id& out_type )
    {
      static std::map< std::string, type_id > types{
        { "i32", type_id::i32 },
        { "bool", type_id::bool_ }
      };
      auto pos = types.find( name );
      if( pos == types.end() )
      {
        return false;
      }
      out_type = pos->second;
      return true;
    }
  }
}
