#pragma once

// std::istreambuf_iterator, std::iterator, iterator tags
#include <iterator>
// instanciating an istreambuf_iterator implicitly instantiates a basic_istream, which is defined here
#include <istream>
// tuple
#include <tuple>
// std::ostream
#include <ostream>
// assertions
#include <cassert>

// boost::spirit::multi_pass
#include <boost/spirit/home/support/iterators/multi_pass.hpp>
// boost::spirit::iterator_policies
#include <boost/spirit/home/support/iterators/detail/no_check_policy.hpp>
#include <boost/spirit/home/support/iterators/detail/ref_counted_policy.hpp>
#include <boost/spirit/home/support/iterators/detail/buffering_input_iterator_policy.hpp>
#include <boost/spirit/home/support/iterators/detail/split_std_deque_policy.hpp>

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

    /// UTF-8 aware file position tracking multipass istreambuff iterator
    class istreambuf_multipass_position_iterator : public std::iterator< std::forward_iterator_tag, char >
    {
    public:
      /// DefaultConstructible: create end iterator
      istreambuf_multipass_position_iterator();
      /// Adapting an istreambuf_multipass_utf32_iterator
      istreambuf_multipass_position_iterator( istreambuf_multipass_iterator current, istreambuf_multipass_iterator end );
      /// Destructible
      ~istreambuf_multipass_position_iterator();
      /// MoveConstructible
      istreambuf_multipass_position_iterator( istreambuf_multipass_position_iterator && rhs );
      /// MoveAssignable
      istreambuf_multipass_position_iterator& operator=( istreambuf_multipass_position_iterator&& rhs );
      /// CopyConstructible
      istreambuf_multipass_position_iterator( const istreambuf_multipass_position_iterator& rhs );
      /// CopyAssignable
      istreambuf_multipass_position_iterator& operator=( const istreambuf_multipass_position_iterator& rhs );
      
      /// EqualityComparable
      bool operator==( const istreambuf_multipass_position_iterator& rhs ) const;
      /// Inequality comparison
      bool operator!=( const istreambuf_multipass_position_iterator& rhs ) const;

      // Element selection through pointer (operator->) omitted since that makes no sense in the context of char32_t

      /// Dereference
      const value_type& operator*() const;

      /// Prefix increment
      istreambuf_multipass_position_iterator& operator++();
      /// Postfix increment
      istreambuf_multipass_position_iterator operator++( int );

      /// Retrieve position in file
      const file_position& get_position() const;

    private:
      file_position _position;
      istreambuf_multipass_iterator _current;
      istreambuf_multipass_iterator _end;
      value_type _value = '\0';
    };

    /// copyable istream iterator yielding utf32 with positional information
    typedef istreambuf_multipass_position_iterator enhanced_istream_iterator;

    /// create utf32-yielding begin & end iterator with positional information for the given stream
    std::tuple< enhanced_istream_iterator, enhanced_istream_iterator > enhanced_iterators( std::istream& stream );

    /**
    @brief Skip byte order mark (U+FEFF) if there is any
    @param it potential BOM location
    @param end end of input
    @return Whether a BOM was encountered
    */
    bool skip_byte_order_mark( enhanced_istream_iterator& it, const enhanced_istream_iterator& end );
  }
}
