#pragma once

#include <functional>
#include <map>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <type_traits>

#include "vm/code_segment.hpp"
#include "vm/instruction_pointer.hpp"

struct get_current_address
{
  get_current_address( perseus::detail::instruction_pointer::value_type& adr ) : address( adr ) {}
  perseus::detail::instruction_pointer::value_type& address;
};

struct label
{
  label( const std::string& name ) : name( name ) {}
  const std::string name;
};

typedef std::map< std::string, std::uint32_t > label_map;

struct label_reference
{
  label_reference( const std::string& name ) : name( name ) {}
  const std::string name;
};

struct label_reference_offset : label_reference
{
  using label_reference::label_reference;
};

namespace detail
{

  /// End of recursion (noop)
  static void add_code( perseus::detail::code_segment&, const label_map& )
  {
  }

  // forward declarations
  template< typename... Tail >
  void add_code( perseus::detail::code_segment& out_code, const label_map& labels, const get_current_address& retriever, Tail... tail );
  template< typename... Tail >
  void add_code( perseus::detail::code_segment& out_code, const label_map& labels, const label_reference& reference, Tail... tail );
  template< typename... Tail >
  void add_code( perseus::detail::code_segment& out_code, const label_map& labels, const label_reference_offset& reference, Tail... tail );
  template< typename... Tail >
  void add_code( perseus::detail::code_segment& out_code, const label_map& labels, const label&, Tail... tail );

  /**
  @brief Add values to a code segment
  @param out_code[in,out] code to append onto
  @param head first value to append
  @param tail remaining values to append
  @note the enable_if is necessary because otherwise this will get instantiated to participate in overload resolution (where it will be discarded in favor of better fits), triggering the static_assert in code.push() for e.g. label
  */
  template< typename Head, typename = typename std::enable_if< std::is_trivially_copyable< Head >::value >::type, typename... Tail >
  void add_code( perseus::detail::code_segment& out_code, const label_map& labels, const Head& head, Tail... tail )
  {
    out_code.push< Head >( head );
    add_code( out_code, labels, tail... );
  }

  /// Specialization: retrieving current address
  template< typename... Tail >
  void add_code( perseus::detail::code_segment& out_code, const label_map& labels, const get_current_address& retriever, Tail... tail )
  {
    retriever.address = out_code.size();
    add_code( out_code, labels, tail... );
  }

  /// Specialization: retrieving label address
  template< typename... Tail >
  void add_code( perseus::detail::code_segment& out_code, const label_map& labels, const label_reference& reference, Tail... tail )
  {
    auto pos = labels.find( reference.name );
    if( pos == labels.end() )
    {
      throw std::logic_error( "invalid label reference" );
    }
    out_code.push< std::uint32_t >( pos->second );
    add_code( out_code, labels, tail... );
  }

  /// Specialization: retrieving label offset
  template< typename... Tail >
  void add_code( perseus::detail::code_segment& out_code, const label_map& labels, const label_reference_offset& reference, Tail... tail )
  {
    auto pos = labels.find( reference.name );
    if( pos == labels.end() )
    {
      throw std::logic_error( "invalid label reference" );
    }
    out_code.push< std::uint32_t >( pos->second - out_code.size() - sizeof( std::uint32_t ) );
    add_code( out_code, labels, tail... );
  }

  /// Specialization: label (ignored)
  template< typename... Tail >
  void add_code( perseus::detail::code_segment& out_code, const label_map& labels, const label&, Tail... tail )
  {
    add_code( out_code, labels, tail... );
  }


  static void calculate_labels( label_map&, const std::uint32_t )
  {
  }

  // forward declarations
  template< typename... Tail >
  static void calculate_labels( label_map& out_labels, const std::uint32_t address, const get_current_address& retriever, const Tail&... tail );
  template< typename... Tail >
  static void calculate_labels( label_map& out_labels, const std::uint32_t address, const label_reference& reference, const Tail&... tail );
  template< typename... Tail >
  static void calculate_labels( label_map& out_labels, const std::uint32_t address, const label_reference_offset& reference, const Tail&... tail );
  template< typename... Tail >
  static void calculate_labels( label_map& out_labels, const std::uint32_t address, const label& lab, const Tail&... tail );

  // the enable_if is necessary because otherwise this will get instantiated to participate in overload resolution (where it will be discarded in favor of better fits), triggering the static_assert in code.push() for e.g. label
  template< typename Head, typename = typename std::enable_if< std::is_trivially_copyable< Head >::value >::type, typename... Tail >
  static void calculate_labels( label_map& out_labels, const std::uint32_t address, const Head& head, const Tail&... tail )
  {
    perseus::detail::code_segment::size_type size = 0;
    {
      // to get the correct size even in case of opcode encoding etc.
      perseus::detail::code_segment code;
      code.push< Head >( head );
      size = code.size();
    }
    calculate_labels( out_labels, address + size, tail... );
  }

  /// special case: retrieving current address (ignored)
  template< typename... Tail >
  static void calculate_labels( label_map& out_labels, const std::uint32_t address, const get_current_address& retriever, const Tail&... tail )
  {
    calculate_labels( out_labels, address, tail... );
  }

  /// special case: retrieving label address (ignored)
  template< typename... Tail >
  static void calculate_labels( label_map& out_labels, const std::uint32_t address, const label_reference& reference, const Tail&... tail )
  {
    calculate_labels( out_labels, address + sizeof( std::uint32_t ), tail... );
  }

  /// special case: retrieving label offset (ignored)
  template< typename... Tail >
  static void calculate_labels( label_map& out_labels, const std::uint32_t address, const label_reference_offset& reference, const Tail&... tail )
  {
    calculate_labels( out_labels, address + sizeof( std::uint32_t ), tail... );
  }

  /// special case: label
  template< typename... Tail >
  static void calculate_labels( label_map& out_labels, const std::uint32_t address, const label& lab, const Tail&... tail )
  {
    if( !out_labels.insert( { lab.name, address } ).second )
    {
      throw std::logic_error( "Duplicate label " + lab.name );
    }
    calculate_labels( out_labels, address, tail... );
  }
}

template< typename... Args >
perseus::detail::code_segment create_code_segment( const Args&... args )
{
  perseus::detail::code_segment code;
  label_map labels;
  detail::calculate_labels( labels, 0u, args... );
  detail::add_code( code, labels, args... );
  return code;
}
