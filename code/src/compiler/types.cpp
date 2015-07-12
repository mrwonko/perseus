#include "compiler/types.hpp"
#include "compiler/exceptions.hpp"
#include "compiler/ast.hpp"

#include <stdexcept>
#include <map>

namespace perseus
{
  namespace detail
  {
    using namespace std::string_literals;

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
      default:
        throw std::logic_error( "invalid type_id" );
      }
    }

    bool get_type( const ast::identifier& name, type_id& out_type )
    {
      static std::map< std::string, type_id > types{
        { "i32"s, type_id::i32 },
        { "bool"s, type_id::bool_ }
      };
      auto pos = types.find( name );
      if( pos == types.end() )
      {
        return false;
      }
      out_type = pos->second;
      return true;
    }

    type_id get_type( const ast::identifier& name )
    {
      type_id id;
      if( !get_type( name, id ) )
      {
        throw semantic_error{ "Invalid type: "s + static_cast< const std::string& >( name ), static_cast< const file_position& >( name ) };
      }
      return id;
    }

    std::string get_name( type_id t )
    {
      switch( t )
      {
      case type_id::void_:
        return "()"s;
      case type_id::bool_:
        return "bool"s;
      case type_id::i32:
        return "i32"s;
      default:
        throw std::logic_error( "invalid type_id" );
      }
    }
  }
}
