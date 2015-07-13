#include "compiler/steps.hpp"
#include "compiler/types.hpp"
#include "compiler/exceptions.hpp"
#include "shared/optional_apply.hpp"

#include <vector>
#include <algorithm>
#include <iterator>
#include <utility>
#include <string>

namespace perseus
{
  namespace detail
  {
    using namespace std::string_literals;

    void extract_functions( ast::parser::file& ast, function_manager& functions )
    {
      for( ast::parser::function_definition& function : ast.functions )
      {
        std::vector< type_id > parameters;
        parameters.reserve( function.arguments.size() );
        std::transform( function.arguments.begin(), function.arguments.end(), std::back_inserter( parameters ), 
          []( const ast::parser::function_argument& arg)
        {
          return get_type( arg.type );
        } );
        function_signature signature{ function.name, std::move( parameters ) };
        function_info info{ optional_apply( function.type, []( const ast::identifier& type ) { return get_type( type ); } ).value_or( type_id::void_ ), function.pure };
        if( !functions.register_function( std::move( signature ), std::move( info ), function.manager_entry ) )
        {
          throw semantic_error{ "Duplicate function definition for "s + static_cast< const std::string& >( function.name ), static_cast< const file_position& >( function.name ) };
        }
      }
    }
  }
}