#pragma once

#include <functional>

#include "vm/code_segment.hpp"
#include "vm/instruction_pointer.hpp"

/// End of recursion (noop)
static void add_code( perseus::detail::code_segment& out_code )
{
}

/**
@brief Add values to a code segment
@param out_code[in,out] code to append onto
@param head first value to append
@param tail remaining values to append
*/
template< typename Head, typename... Tail >
void add_code( perseus::detail::code_segment& out_code, const Head& head, Tail... tail )
{
  out_code.push< Head >( head );
  add_code( out_code, tail... );
}

struct get_current_address
{
  get_current_address( perseus::detail::instruction_pointer::value_type& adr ) : address( adr ) {}
  perseus::detail::instruction_pointer::value_type& address;
};

/// Specialization: retrieving current address
template< typename... Tail >
void add_code( perseus::detail::code_segment& out_code, const get_current_address& retriever, Tail... tail )
{
  retriever.address = out_code.size();
  add_code( out_code, tail... );
}

template< typename... Args >
perseus::detail::code_segment create_code_segment( Args... args )
{
  perseus::detail::code_segment code;
  add_code( code, args... );
  return code;
}
