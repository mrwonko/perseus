#pragma once

#include <boost/optional/optional.hpp>

#include <utility>

template< typename A, typename F >
auto optional_apply( A&& val, F func ) -> decltype( boost::make_optional( func( *std::forward< A >( val ) ) ) )
{
  if( val )
  {
    return boost::make_optional( func( *std::forward< A >( val ) ) );
  }
  else
  {
    return{};
  }
}
