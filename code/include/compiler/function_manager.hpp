#pragma once

#include <map>
#include <vector>
#include <string>
#include <cstdint>
#include <utility>

#include <boost/optional/optional.hpp>

#include "types.hpp"
#include "shared/opcodes.hpp"
#include "vm/instruction_pointer.hpp"

namespace perseus
{
  namespace detail
  {
    class code_segment;

    enum class operator_associativity
    {
      none,
      left,
      right
    };

    enum
    {
      default_precedence = 9
    };

    // TODO: this won't work for typeclasses; well, the whole type system won't.
    struct function_signature
    {
      std::string name;
      std::vector< type_id > parameters;

      bool operator<( const function_signature& rhs ) const;
      std::vector< type_id >::size_type parameters_size() const;
    };

    class function_info
    {
    public:
      /// constructor
      function_info( type_id return_type, unsigned int precedence = default_precedence, operator_associativity associativity = operator_associativity::left )
        : return_type( return_type )
        , precedence( precedence )
        , associativity( associativity )
      {
      }
      /// builtin function constructor
      function_info( opcode code, type_id return_type, unsigned int precedence = default_precedence, operator_associativity associativity = operator_associativity::left )
        : return_type( return_type )
        , precedence( precedence )
        , associativity( associativity )
        , _opcode( code )
      {
      }
      /// Move constructible
      function_info( function_info&& ) = default;
      /// Move assignable
      function_info& operator=( function_info&& ) = default;
      /// Non-Copyable
      function_info( const function_info& ) = delete;
      /// Non-Copyable
      function_info& operator=( const function_info& ) = delete;

      const type_id return_type;
      const unsigned int precedence;
      const operator_associativity associativity;
      
      /**
      @brief Fulfils address requests with the given address
      */
      void set_address( instruction_pointer::value_type address, code_segment& code );
      /**
      @brief Writes this function's address to the given code segment, or stores the request and writes a placeholder if the address is not yet known.
      */
      void write_address( code_segment& code );
      
      bool get_opcode( opcode& out_result ) const
      {
        if( _opcode.is_initialized() )
        {
          out_result = *_opcode;
          return true;
        }
        return false;
      }

      bool has_requests() const
      {
        return !_address_requests.empty();
      }

      bool address_set() const
      {
        return _address.is_initialized();
      }

    private:
      /// for builtin functions, this is the opcode that can be issued in lieu of a call
      boost::optional< opcode > _opcode;
      boost::optional< instruction_pointer::value_type > _address;
      /// until the @ref address is known, requests for the address return a placeholder and have their address stored here, so they can be changed once the address is known.
      std::vector< instruction_pointer::value_type > _address_requests;
    };

    class function_manager
    {
    public:
      typedef std::map< function_signature, function_info > function_map;

      /**
      Constructor. Registers built-in functions.
      @todo At some point I'll probably need multiple function managers, one per scope, at which point built-in functions should no longer be added by default.
      */
      function_manager();
      /// Move constructible
      function_manager( function_manager&& ) = default;
      /// Move assignable
      function_manager& operator=( function_manager&& ) = default;
      /// non-copyable
      function_manager( const function_manager& ) = delete;
      /// non-copyable
      function_manager& operator=( const function_manager& ) = delete;
      
      /**
      @brief Registers a function, i.e. declares its signature valid
      */
      boost::optional< function_map::const_iterator > register_function( function_signature&& signature, function_info&& info );
      
      function_info& get_function( const function_signature& function )
      {
        return _functions.at( function );
      }
      
      const function_info& get_function( const function_signature& function ) const
      {
        return _functions.at( function );
      }
      
      bool has_open_address_requests() const;
      
      void write_builtin_functions( code_segment& code );

      /**
      @brief Retrieves all functions of the given name.
      @returns iterator range, i.e. tuple(begin, end)
      */
      std::pair< function_map::const_iterator, function_map::const_iterator > get_functions( const std::string& name ) const;
    private:
      function_map _functions;
    };
  }
}
