#include "compiler/compile.hpp"
#include "utf8.h"

#include <boost/spirit/home/classic/iterator/position_iterator.hpp>
#include <boost/spirit/home/support/iterators/multi_pass.hpp>
#include <boost/spirit/home/support/iterators/istream_iterator.hpp>
#include <boost/spirit/home/support/iterators/detail/no_check_policy.hpp>
#include <boost/spirit/home/support/iterators/detail/first_owner_policy.hpp>
#include <boost/spirit/home/support/iterators/detail/buffering_input_iterator_policy.hpp>
#include <boost/spirit/home/support/iterators/detail/split_std_deque_policy.hpp>

#include <iostream>
#include <iomanip>

namespace perseus
{
  void compile( std::istream& source_stream, const std::string& filename )
  {
    // iterator over underlying buffer of istream (i.e. raw stream data)
    typedef std::istreambuf_iterator< char > istream_iterator;
    istream_iterator istream_begin( source_stream );
    istream_iterator istream_end;

    // adapt iterator to be copyable
    typedef boost::spirit::multi_pass<
      istream_iterator,
      boost::spirit::iterator_policies::default_policy<
      boost::spirit::iterator_policies::first_owner, // the oldest iterator owns the backtrack buffer, copies reference that (works since copies are only on stack, so FILO, no ref count needed)
      boost::spirit::iterator_policies::no_check, // clear_queue() calls may invalidate iterator copies, don't check for that to speed things up, our usage is safe
      boost::spirit::iterator_policies::buffering_input_iterator, // additionally buffer the latest character, since istreambuf_iterator does not do that
      boost::spirit::iterator_policies::split_std_deque // keep previous values in a variably sized queue (as opposed to a fixed-size circular buffer)
      >
    > multipass_iterator;
    multipass_iterator multipass_begin( istream_begin );
    multipass_iterator multipass_end( istream_end );

    // convert utf8 to (utf32) code points, so the position can be calculated correctly (for the most part; unicode still has some quirks this doesn't account for, but it's close enough)
    typedef utf8::iterator< multipass_iterator > utf8_iterator;
    utf8_iterator utf8_begin( multipass_begin, multipass_begin, multipass_end );
    utf8_iterator utf8_end( multipass_end, multipass_begin, multipass_end );

    // adapt iterator to include positional information (for error messages)
    typedef boost::spirit::classic::position_iterator< utf8_iterator, boost::spirit::classic::file_position > position_iterator;
    position_iterator position_begin( utf8_begin, utf8_end, filename );
    position_iterator position_end;

    // temp test: print items with line info
    for( position_iterator it = position_begin; it != position_end; ++it )
    {
      std::cout
        << it.get_position().file << ":" << it.get_position().line << ":" << it.get_position().column << ": "
        << char( *it ) << " (\\U" << std::setfill( '0' ) << std::setw( 8 ) << std::hex << *it << ")" << std::endl;
    }
  }
}
