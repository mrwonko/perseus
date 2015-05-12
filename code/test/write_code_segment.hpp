#pragma once

#include "vm/code_segment.hpp"

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
void add_code( perseus::detail::code_segment& out_code, Head head, Tail... tail )
{
  out_code.push< Head >( head );
  add_code( out_code, tail... );
}

template< typename... Args >
perseus::detail::code_segment create_code_segment( Args... args )
{
  perseus::detail::code_segment code;
  add_code( code, args... );
  return code;
}
