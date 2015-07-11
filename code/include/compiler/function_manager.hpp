#pragma once

#include <map>
#include <vector>
#include <string>
#include <cstdint>

#include <boost/optional/optional.hpp>

#include "types.hpp"
#include "shared/opcodes.hpp"

namespace perseus
{
  namespace detail
  {
    struct code_segment;

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
    };

    struct function_info
    {
      type_id return_type;
      unsigned int precedence = default_precedence;
      operator_associativity associativity = operator_associativity::left;
      /// for builtin functions, this is the opcode that can be issued in lieu of a call
      boost::optional< opcode > call_opcode;
      boost::optional< std::uint32_t > address;
      /// until the @ref address is known, requests for the address return a placeholder and have their address stored here, so they can be changed once the address is known.
      std::vector< std::uint32_t > address_requests;
      
      /**
      @brief Fulfils address requests with the given address
      */
      void set_address( std::uint32_t address, code_segment& code );
      /**
      @brief Writes this function's address to the given code segment, or stores the request and writes a placeholder if the address is not yet known.
      */
      void write_address( code_segment& code );
      
      bool is_builtin() const
      {
        return call_opcode;
      }
    };

    class function_manager
    {
    public:
      typedef std::map< function_signature, function_info > function_map;

      /// Default constructible
      function_manager() = default;
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
      void register_function( function_signature&& signature, function_info&& info );
      
      function_info& get_function( const function_signature& function );
      
      const function_info& get_function( const function_signature& function ) const;
      
      bool has_open_address_requests() const;
      
      void write_builtin_functions( code_segment& code );

      /**
      @brief Retrieves all functions of the given name.
      @returns iterator range, i.e. tuple(begin, end)
      */
      std::tuple< function_map::const_iterator, function_map::const_iterator > get_functions( const std::string& name ) const;
    private:
      function_map _functions;
    };
  }
}
