#include "compiler/function_manager.hpp"
#include "vm/code_segment.hpp"

#include <tuple>

namespace perseus
{
  namespace detail
  {
    bool function_signature::operator<( const function_signature& rhs ) const
    {
      return std::tie( name, parameters ) < std::tie( rhs.name, rhs.parameters );
    }

    code_segment generate_builtin_operation( opcode operation, std::uint32_t argument_size, std::uint32_t return_size )
    {
      const std::uint32_t address_size = sizeof( instruction_pointer::value_type );
      code_segment result;
      // stack: <result memory> <arguments> <return address>
      result.push< opcode >( opcode::relative_load_stack );
      result.push< std::uint32_t >( argument_size );
      result.push< std::int32_t >( -argument_size - address_size );
      // stack: <result memory> <arguments> <return address> <arguments>
      result.push< opcode >( operation );
      // stack: <result memory> <arguments> <return address> <result>
      result.push< opcode >( opcode::relative_store_stack );
      result.push< std::uint32_t >( result_size );
      result.push< std::int32_t >( -2 * result_size - adress_size -argument_size );
      // stack: <result> <arguments> <return address> <result>
      result.push< opcode >( opcode::pop );
      result.push< std::uint32_t >( result_size );
      // stack: <result> <arguments> <return address>
      result.push< opcode >( opcode::return_ );
      result.push< std::uint32_t >( argument_size );
      // stack: <result>
      return result;
    }

    typedef std::tuple< function_signature, function_info, opcode > builtin_function;
    static const builtin_function builtin_functions[] = {
      builtin_function{
        { "+", { type_id::i32, type_id::i32 } },
        { type_id::i32, 6, operator_associativity::left },
        opcode::add_i32
      }
    };

    std::tuple< function_manager::function_map::const_iterator, function_manager::function_map::const_iterator > function_manager::get_functions( const std::string& name ) const
    {
      // this works because it's primarily sorted by name, i.e. equal names are neighbors
      function_map::const_iterator begin = _functions.lower_bound( { name, {} } );
      function_map::const_iterator end = begin;
      while( end != _functions.end() && end->first.name == name )
      {
        ++end;
      }
      return std::tie( begin, end );
    }
  }
}
