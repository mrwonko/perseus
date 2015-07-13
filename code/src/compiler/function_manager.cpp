#include "compiler/function_manager.hpp"
#include "vm/code_segment.hpp"

#include <tuple>
#include <cassert>

namespace perseus
{
  namespace detail
  {
    bool function_signature::operator<( const function_signature& rhs ) const
    {
      return std::tie( name, parameters ) < std::tie( rhs.name, rhs.parameters );
    }

    std::vector< type_id >::size_type function_signature::parameters_size() const
    {
      std::vector< type_id >::size_type size = 0;
      for( type_id parameter : parameters )
      {
        size += get_size( parameter );
      }
      return size;
    }

    std::ostream& operator<<( std::ostream& os, const function_signature& signature )
    {
      os << static_cast< const std::string& >( signature.name ) << "(";
      for( auto it = signature.parameters.begin(); it != signature.parameters.end(); )
      {
        os << get_name( *it );
        ++it;
        if( it != signature.parameters.end() )
        {
          os << ", ";
        }
      }
      os << ")";
      return os;
    }

    void function_info::set_address( instruction_pointer::value_type address, code_segment& code )
    {
      assert( !address_set() );
      _address = address;
      for( const instruction_pointer::value_type& request : _address_requests )
      {
        code.write( request, address );
      }
      _address_requests.clear();
    }

    void function_info::write_address( code_segment& code )
    {
      if( address_set() )
      {
        code.push( *_address );
      }
      else
      {
        // address not yet known, push a placeholder and store the request
        _address_requests.push_back( code.size() );
        code.push( instruction_pointer::value_type{} );
      }
    }

    static void generate_builtin_operation( opcode operation, std::uint32_t argument_size, std::uint32_t return_size, code_segment& code )
    {
      const std::uint32_t address_size = sizeof( instruction_pointer::value_type );
      // stack: <result memory> <arguments> <return address>
      // copy arguments to top of stack
      code.push( opcode::relative_load_stack );
      code.push( argument_size );
      code.push( -static_cast< std::int32_t >( argument_size ) - static_cast< std::int32_t >( address_size ) );
      // stack: <result memory> <arguments> <return address> <arguments>
      // invoke operation
      code.push( operation );
      // stack: <result memory> <arguments> <return address> <result>
      // write result to result memory
      code.push( opcode::relative_store_stack );
      code.push( return_size );
      code.push( -2 * static_cast< std::int32_t >( return_size ) - static_cast< std::int32_t >( address_size ) - static_cast< std::int32_t >( argument_size ) );
      // stack: <result> <arguments> <return address> <result>
      // pop result
      code.push( opcode::pop );
      code.push( return_size );
      // stack: <result> <arguments> <return address>
      // return, popping arguments
      code.push( opcode::return_ );
      code.push( argument_size );
      // stack: <result>
    }
    
    function_manager::function_manager()
    {
      // function_signature{ identifier, { parameter types } }, function_info{ opcode, return type, precedence, associativity }
      _functions.emplace( function_signature{ "+",{ type_id::i32, type_id::i32 } }, function_info{ opcode::add_i32, type_id::i32, true, 6, operator_associativity::left } );
    }

    boost::optional< function_manager::function_map::const_iterator > function_manager::register_function( function_signature&& signature, function_info&& info )
    {
      function_map::const_iterator it;
      bool inserted;
      std::tie( it, inserted ) = _functions.emplace( std::make_pair( std::move( signature ), std::move( info ) ) );
      return inserted ? boost::make_optional( it ) : boost::optional< function_map::const_iterator >{};
    }

    boost::optional< function_manager::function_map::const_iterator > function_manager::get_function( const function_signature& function ) const
    {
      function_map::const_iterator pos = _functions.find( function );
      return pos == _functions.end() ? boost::optional< function_map::const_iterator >{} : boost::make_optional( pos );
    }

    bool function_manager::has_open_address_requests() const
    {
      return std::any_of( _functions.begin(), _functions.end(), []( const function_map::value_type& entry ) { return entry.second.has_requests(); } );
    }

    void function_manager::write_builtin_functions( code_segment& code )
    {
      for( auto& entry : _functions )
      {
        function_info& info = entry.second;
        assert( !info.address_set() );
        opcode op;
        if( info.get_opcode( op ) )
        {
          info.set_address( code.size(), code );
          generate_builtin_operation( op, entry.first.parameters_size(), get_size( info.return_type ), code );
        }
      }
    }

    std::pair< function_manager::function_map::const_iterator, function_manager::function_map::const_iterator > function_manager::get_functions( const std::string& name ) const
    {
      // this works because it's primarily sorted by name, i.e. equal names are neighbors
      function_map::const_iterator begin = _functions.lower_bound( { name, {} } );
      function_map::const_iterator end = begin;
      while( end != _functions.end() && end->first.name == name )
      {
        ++end;
      }
      return std::make_pair( begin, end );
    }
  }
}
