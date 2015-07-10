#include "compiler/iterators.hpp"

namespace perseus
{
  namespace detail
  {

    istreambuf_multipass_position_iterator::istreambuf_multipass_position_iterator()
    {
    }
    /// Adapting an istreambuf_multipass_utf32_iterator
    istreambuf_multipass_position_iterator::istreambuf_multipass_position_iterator( istreambuf_multipass_iterator current, istreambuf_multipass_iterator end )
      : _current( current ), _end( end ), _value( current == end ? '\0' : *current )
    {
    }
    /// Destructible
    istreambuf_multipass_position_iterator::~istreambuf_multipass_position_iterator() = default;
    /// MoveConstructible
    istreambuf_multipass_position_iterator::istreambuf_multipass_position_iterator( istreambuf_multipass_position_iterator && rhs )
      : _current( rhs._current ), _end( rhs._end ), _position( rhs._position ), _value( rhs._value )
    {
      rhs._current = istreambuf_multipass_iterator{};
      rhs._end = istreambuf_multipass_iterator{};
    }
    /// MoveAssignable
    istreambuf_multipass_position_iterator& istreambuf_multipass_position_iterator::operator=( istreambuf_multipass_position_iterator&& rhs )
    {
      _current = rhs._current;
      _end = rhs._end;
      _position = rhs._position;
      _value = rhs._value;
      rhs._current = istreambuf_multipass_iterator{};
      rhs._end = istreambuf_multipass_iterator{};
      return *this;
    }
    /// CopyConstructible
    istreambuf_multipass_position_iterator::istreambuf_multipass_position_iterator( const istreambuf_multipass_position_iterator& rhs )
      : _current( rhs._current ), _end( rhs._end ), _position( rhs._position ), _value( rhs._value )
    {
    }
    /// CopyAssignable
    istreambuf_multipass_position_iterator& istreambuf_multipass_position_iterator::operator=( const istreambuf_multipass_position_iterator& rhs )
    {
      _current = rhs._current;
      _end = rhs._end;
      _position = rhs._position;
      _value = rhs._value;
      return *this;
    }

    /// EqualityComparable
    bool istreambuf_multipass_position_iterator::operator==( const istreambuf_multipass_position_iterator& rhs ) const
    {
      return std::tie( _current, _end ) == std::tie( rhs._current, rhs._end );
    }

    /// Inequality comparison
    bool istreambuf_multipass_position_iterator::operator!=( const istreambuf_multipass_position_iterator& rhs ) const
    {
      return !( ( *this ) == rhs );
    }

    // Element selection through pointer omitted since that makes no sense in the context of char32_t

    /// Dereference
    const istreambuf_multipass_position_iterator::value_type& istreambuf_multipass_position_iterator::operator*() const
    {
      return _value;
    }

    /// Postfix increment
    istreambuf_multipass_position_iterator istreambuf_multipass_position_iterator::operator++( int )
    {
      istreambuf_multipass_position_iterator res( *this );
      ++( *this );
      return res;
    }

    /// Retrieve position in file
    const file_position& istreambuf_multipass_position_iterator::get_position() const
    {
      return _position;
    }

    istreambuf_multipass_position_iterator& istreambuf_multipass_position_iterator::operator++()
    {
      assert( _current != _end );
      if( _value == '\n' )
      {
        _position.line += 1;
        _position.column = 0;
      }
      ++_current;
      if( _current != _end )
      {
        _value = *_current;
        // don't count continuation bytes
        if( ( _value & 0b1100'0000 ) != 0b1000'0000 )
        {
          _position.column += 1;
        }
      }
      return *this;
    }

    std::tuple< enhanced_istream_iterator, enhanced_istream_iterator > enhanced_iterators( std::istream& stream )
    {
      istreambuf_multipass_iterator multipass_begin{ istreambuf_iterator{ stream } };
      istreambuf_multipass_iterator multipass_end{ istreambuf_iterator{} };
      enhanced_istream_iterator begin{ multipass_begin, multipass_end };
      enhanced_istream_iterator end;
      return std::make_tuple( begin, end );
    }

    bool skip_byte_order_mark( enhanced_istream_iterator& it, const enhanced_istream_iterator& end )
    {
      const detail::enhanced_istream_iterator begin = it;
      if(
        ( it != end && *it++ == '\xEF' ) &&
        ( it != end && *it++ == '\xBB' ) &&
        ( it != end && *it++ == '\xBF' )
        )
      {
        return true;
      }
      else
      {
        it = begin;
        return false;
      }
    }
  }
}
