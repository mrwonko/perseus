#pragma once

#include <string>
#include <cstdint>

namespace perseus
{
  namespace detail
  {
    // there are no user-defined types yet (and they'll be a PITA), so I'm keeping it super simple for now.

    enum class type_id
    {
      void_,
      i32,
      bool_
    };

    std::uint32_t get_size( type_id t );

    bool get_type( const std::string& name, type_id& out_type );

    std::string get_name( type_id t );
  }
}
