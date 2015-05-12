#include "vm/coroutine_manager.hpp"

#include <cassert>
#include <climits>
#include <string>

namespace perseus
{
  namespace detail
  {
    coroutine& coroutine_manager::new_coroutine( const code_segment& code, const instruction_pointer::value_type start_address, stack&& initial_stack )
    {
      coroutine::identifier index = _next_index;
      // reuse indices of deleted coroutines, if any
      if( _free_indices.empty() )
      {
        // if a user actually manages to create this many coroutines he's doing something wrong.
        if( _next_index == std::numeric_limits< coroutine::identifier >::max() )
        {
          throw too_many_coroutines( "Exceeded limit of " + std::to_string( std::numeric_limits< coroutine::identifier >::max() ) + " simultaneous coroutines." );
        }
        _next_index += 1;
      }
      else
      {
        index = _free_indices.back();
        _free_indices.pop_back();
      }
      auto result = _coroutines.emplace( std::make_pair( index, coroutine( index, code, start_address, std::move( initial_stack ) ) ) );
      assert( result.second );
      return result.first->second;
    }

    void coroutine_manager::delete_coroutine( const coroutine::identifier index )
    {
      auto iter = _coroutines.find( index );
      if( iter == _coroutines.end() )
      {
        throw invalid_coroutine_identifer( "Invalid coroutine identifier used for deletion!" );
      }
      if( iter->second.current_state == coroutine::state::live )
      {
        throw deleting_live_coroutine( "Trying to delete live coroutine!" );
      }
      _coroutines.erase( iter );
    }

    coroutine& coroutine_manager::get_coroutine( const coroutine::identifier index )
    {
      auto iter = _coroutines.find( index );
      if( iter == _coroutines.end() )
      {
        throw invalid_coroutine_identifer( "Invalid coroutine identifier used for deletion!" );
      }
      return iter->second;
    }

    const coroutine& coroutine_manager::get_coroutine( const coroutine::identifier index ) const
    {
      auto iter = _coroutines.find( index );
      if( iter == _coroutines.end() )
      {
        throw invalid_coroutine_identifer( "Invalid coroutine identifier used for deletion!" );
      }
      return iter->second;
    }

    void coroutine_manager::clear()
    {
      // this is only called by opcode::exit and its variants, so we don't check if any coroutines are still live.
      _coroutines.clear();
      _free_indices.clear();
      _next_index = 0;
    }
  }
}
