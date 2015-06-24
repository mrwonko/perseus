#pragma once

#include "coroutine.hpp"

#include <map>
#include <vector>

namespace perseus
{
  namespace detail
  {
    /**
    @brief Manager responsible for @ref coroutine identifier allocation.

    Also stores the coroutines.
    @todo allow custom allocator? (remove references to std::bad_alloc then)
    */
    class coroutine_manager
    {
    public:
      /// constructor
      coroutine_manager() = default;
      /// non-copyable
      coroutine_manager( const coroutine_manager& ) = delete;
      /// non-copyable
      coroutine_manager& operator=( const coroutine_manager& ) = delete;
      /// move constructor
      coroutine_manager( coroutine_manager&& ) = default;
      /// move assignment
      coroutine_manager& operator=( coroutine_manager&& ) = default;

      /**
      @brief Create a new coroutine
      
      The new coroutine is assigned the lowest free identifier

      @param code @ref code_segment the coroutine operates in
      @param start_address address in the code to start execution at
      @param initial_stack Initial values on the stack of the coroutine
      @returns the new coroutine
      @throws std::bad_alloc if the system runs out of memory
      @throws too_many_coroutines if we run out of identifiers (basically only possible on incorrect usage)
      @pre The total number of coroutines fits into @ref coroutine::identifier
      */
      coroutine& new_coroutine( const code_segment& code, const instruction_pointer::value_type start_address, stack&& initial_stack = stack() );

      /**
      @brief Delete a given @ref coroutine by identifier.
      @param index Identifier of the coroutine to delete
      @throws invalid_coroutine_identifier if no such coroutine exists
      @throws deleting_live_coroutine if trying to delete a live coroutine (i.e. the active one or one waiting on it)
      */
      void delete_coroutine( const coroutine::identifier index );

      /**
      @brief Retrieves a @ref coroutine by identifier
      @param index identifier of the @ref coroutine to retrieve
      @returns the @ref coroutine with the given identifier
      @throws invalid_coroutine_identifier if no such coroutine exists
      */
      coroutine& get_coroutine( const coroutine::identifier index );

      /// @ref delete_coroutine() for const coroutine_manager.
      const coroutine& get_coroutine( const coroutine::identifier index ) const;

      /**
      @brief Whether there are no coroutines.
      */
      bool empty() const
      {
        return _coroutines.empty();
      }

      /**
      @brief Deletes all coroutines.
      */
      void clear();

    private:
      std::map< coroutine::identifier, coroutine > _coroutines;
      std::vector< coroutine::identifier > _free_indices;
      coroutine::identifier _next_index = 0;
    };
  }
}
