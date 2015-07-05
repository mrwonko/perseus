#pragma once

// std::istreambuf_iterator
#include <iterator>
// tuple
#include <tuple>
// std::ostream
#include <ostream>
// std::iterator, iterator tags
#include <iterator>
// assertions
#include <cassert>

// boost::spirit::multi_pass
#include <boost/spirit/home/support/iterators/multi_pass.hpp>
// boost::spirit::iterator_policies
#include <boost/spirit/home/support/iterators/detail/no_check_policy.hpp>
#include <boost/spirit/home/support/iterators/detail/ref_counted_policy.hpp>
#include <boost/spirit/home/support/iterators/detail/buffering_input_iterator_policy.hpp>
#include <boost/spirit/home/support/iterators/detail/split_std_deque_policy.hpp>

// utf8::iterator
#include "utf8.h"

namespace perseus
{
  namespace detail
  {
    /// istream iterator
    typedef std::istreambuf_iterator< char > istreambuf_iterator;

    /// copyable istream iterator
    typedef boost::spirit::multi_pass<
      istreambuf_iterator,
      boost::spirit::iterator_policies::default_policy<
      boost::spirit::iterator_policies::ref_counted, // since the iterator must still work after this function returns, first_owner doesn't cut it (the first owner is a local in this function)
      boost::spirit::iterator_policies::no_check, // clear_queue() calls may invalidate iterator copies, don't check for that to speed things up, our usage is safe
      boost::spirit::iterator_policies::buffering_input_iterator, // additionally buffer the latest character, since istreambuf_iterator does not do that
      boost::spirit::iterator_policies::split_std_deque // keep previous values in a variably sized queue (as opposed to a fixed-size circular buffer)
      >
    > istreambuf_multipass_iterator;

    /// copyable istream iterator yielding utf32
    typedef utf8::iterator< istreambuf_multipass_iterator > istreambuf_multipass_utf32_iterator;

    /// position in a file
    struct file_position
    {
      /// default constructor
      file_position() = default;
      /// constructor
      file_position( std::size_t line, std::size_t column )
        : line( line ), column( column )
      {
      }
      /// current line
      std::size_t line = 1;
      /// current column (i.e. character in current line)
      std::size_t column = 1;

      /// file_position comparison
      bool operator==( const file_position& rhs ) const
      {
        return std::tie( line, column ) == std::tie( rhs.line, rhs.column );
      }
    };

    /// Overload for displaying @ref file_position
    inline std::ostream& operator<<( std::ostream& os, const file_position& pos )
    {
      return os << pos.line << ':' << pos.column;
    }

    /// iterator 
    class istreambuf_multipass_utf32_position_iterator : public std::iterator< std::forward_iterator_tag, const char32_t >
    {
    public:
      /// DefaultConstructible: create end iterator
      istreambuf_multipass_utf32_position_iterator()
      {
      }
      /// Adapting an istreambuf_multipass_utf32_iterator
      istreambuf_multipass_utf32_position_iterator( istreambuf_multipass_utf32_iterator current, istreambuf_multipass_utf32_iterator end )
        : _current( current ), _end( end ), _value( current == end ? U'\0' : *current )
      {
      }
      /// Destructible
      ~istreambuf_multipass_utf32_position_iterator() = default;
      /// MoveConstructible
      istreambuf_multipass_utf32_position_iterator( istreambuf_multipass_utf32_position_iterator && rhs )
        : _current( rhs._current ), _end( rhs._end ), _position( rhs._position ), _value( rhs._value )
      {
        rhs._current = istreambuf_multipass_utf32_iterator{};
        rhs._end = istreambuf_multipass_utf32_iterator{};
      }
      /// MoveAssignable
      istreambuf_multipass_utf32_position_iterator& operator=( istreambuf_multipass_utf32_position_iterator&& rhs )
      {
        _current = rhs._current;
        _end = rhs._end;
        _position = rhs._position;
        _value = rhs._value;
        rhs._current = istreambuf_multipass_utf32_iterator{};
        rhs._end = istreambuf_multipass_utf32_iterator{};
        return *this;
      }
      /// CopyConstructible
      istreambuf_multipass_utf32_position_iterator( const istreambuf_multipass_utf32_position_iterator& rhs )
        : _current( rhs._current ), _end( rhs._end ), _position( rhs._position ), _value( rhs._value )
      {
      }
      /// CopyAssignable
      istreambuf_multipass_utf32_position_iterator& operator=( const istreambuf_multipass_utf32_position_iterator& rhs )
      {
        _current = rhs._current;
        _end = rhs._end;
        _position = rhs._position;
        _value = rhs._value;
        return *this;
      }
      
      /// EqualityComparable
      bool operator==( const istreambuf_multipass_utf32_position_iterator& rhs ) const
      {
        return std::tie( _current, _end ) == std::tie( rhs._current, rhs._end );
      }

      /// Inequality comparison
      bool operator!=( const istreambuf_multipass_utf32_position_iterator& rhs ) const
      {
        return !( ( *this ) == rhs );
      }

      // Element selection through pointer omitted since that makes no sense in the context of char32_t

      /// Dereference
      reference operator*() const
      {
        return _value;
      }

      /// Prefix increment
      istreambuf_multipass_utf32_position_iterator& operator++()
      {
        assert( _current != _end );
        if( _value == U'\n' )
        {
          _position.line += 1;
          _position.column = 1;
        }
        else
        {
          _position.column += 1;
        }
        ++_current;
        if( _current != _end )
        {
          _value = *_current;
        }
        return *this;
      }

      /// Postfix increment
      istreambuf_multipass_utf32_position_iterator operator++( int )
      {
        istreambuf_multipass_utf32_position_iterator res( *this );
        ++( *this );
        return res;
      }

      /// Retrieve position in file
      const file_position& get_position() const
      {
        return _position;
      }

    private:
      file_position _position;
      istreambuf_multipass_utf32_iterator _current;
      istreambuf_multipass_utf32_iterator _end;
      char32_t _value = U'\0';
    };

    /// copyable istream iterator yielding utf32 with positional information
    typedef istreambuf_multipass_utf32_position_iterator enhanced_istream_iterator;

    /// create utf32-yielding begin & end iterator with positional information for the given stream
    inline std::tuple< enhanced_istream_iterator, enhanced_istream_iterator > enhanced_iterators( std::istream& stream )
    {
      istreambuf_multipass_iterator multipass_begin{ istreambuf_iterator{ stream } };
      istreambuf_multipass_iterator multipass_end{ istreambuf_iterator{} };
      istreambuf_multipass_utf32_iterator utf_begin{ multipass_begin, multipass_begin, multipass_end };
      istreambuf_multipass_utf32_iterator utf_end{ multipass_end, multipass_begin, multipass_end };
      enhanced_istream_iterator begin{ utf_begin, utf_end };
      enhanced_istream_iterator end{ utf_end, utf_end };
      return std::make_tuple( begin, end );
    }
  }
}