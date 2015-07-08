#pragma once

#include "ast.hpp"
#include "token_definitions.hpp"
#include "compiler/exceptions.hpp"

#include <boost/spirit/home/qi/parser.hpp>
#include <boost/spirit/home/support/context.hpp>
#include <boost/spirit/home/qi/skip_over.hpp>

namespace perseus
{
  namespace detail
  {
    struct parsed_string_literal : ast::string_literal
    {
      parsed_string_literal() = default;
      parsed_string_literal( const enhanced_istream_iterator& begin, const enhanced_istream_iterator& end );
    };

    template< typename T, T base >
    T parse_integer_literal( const enhanced_istream_iterator& begin, const enhanced_istream_iterator& end, enhanced_istream_iterator it );

    template< char prefix, char alt_prefix >
    enhanced_istream_iterator consume_prefix( const enhanced_istream_iterator& begin, const enhanced_istream_iterator& end );

    template< typename T, token_id::token_id id, T base, bool with_prefix = false, char prefix = '\0', char alt_prefix = '\0' >
    struct integer_literal_parser : boost::spirit::qi::primitive_parser< integer_literal_parser< T, id, base, with_prefix, prefix, alt_prefix > >
    {
      /// meta-information about the resulting attribute
      template< typename Context, typename Iterator >
      struct attribute
      {
        /// this parser yields an integer
        typedef T type;
      };

      template< typename Context, typename Skipper, typename Attribute >
      bool parse( token_iterator& it, const token_iterator& end, Context&, const Skipper& skipper, Attribute& out_attribute ) const
      {
        boost::spirit::qi::skip_over( it, end, skipper );
        if( it == end )
        {
          return false;
        }
        const token& t = *it;
        if( t.id() != id )
        {
          return false;
        }
        // T is an AST struct adding position information to a value
        out_attribute = parse_integer_literal< typename T, base >(
          t.value().begin(),
          t.value().end(),
          with_prefix ? consume_prefix< prefix, alt_prefix >( t.value().begin(), t.value().end() ) : t.value().begin()
          );
        ++it;
        return true;
      }

      template< typename Context >
      boost::spirit::qi::info what( Context& )
      {
        return{ "integer literal" };
      }
    };

    typedef integer_literal_parser< std::int32_t, token_id::decimal_integer, 10 > decimal_integer_literal_parser;
    typedef integer_literal_parser< std::int32_t, token_id::binary_integer, 2, true, 'b', 'B' > binary_integer_literal_parser;
    typedef integer_literal_parser< std::int32_t, token_id::hexadecimal_integer, 16, true, 'x', 'X' > hexadecimal_integer_literal_parser;

    template< typename T, T base >
    T parse_integer_literal( const enhanced_istream_iterator& begin, const enhanced_istream_iterator& end, enhanced_istream_iterator it )
    {
      T result{};
      integer_literal_overflow up( "integer literal "s + ( base == 2 ? "0b" : base == 16 ? "0x" : "" ) + std::string( begin, end ) + " too large!", begin.get_position() );
      auto add = [ &result, &up ]( const T value )
      {
        if( result > std::numeric_limits< T >::max() - value )
        {
          throw up;
        }
        result += value;
      };
      while( it != end )
      {
        char c = *it++;
        // skip delimiters
        if( c == '\'' )
        {
          continue;
        }
        if( result > std::numeric_limits< T >::max() / base )
        {
          throw up;
        }
        result *= base;
        switch( c )
        {
        case '0':
          break;
        case '1':
          add( 1 );
          break;
        case '2':
          add( 2 );
          break;
        case '3':
          add( 3 );
          break;
        case '4':
          add( 4 );
          break;
        case '5':
          add( 5 );
          break;
        case '6':
          add( 6 );
          break;
        case '7':
          add( 7 );
          break;
        case '8':
          add( 8 );
          break;
        case '9':
          add( 9 );
          break;
        case 'a':
        case 'A':
          add( 10 );
          break;
        case 'b':
        case 'B':
          add( 11 );
          break;
        case 'c':
        case 'C':
          add( 12 );
          break;
        case 'd':
        case 'D':
          add( 13 );
          break;
        case 'e':
        case 'E':
          add( 14 );
          break;
        case 'f':
        case 'F':
          add( 15 );
          break;
        default:
          throw std::logic_error( "can't convert invalid character " + std::to_string( c ) );
        }
      }
      return result;
    }

    template< char prefix, char alt_prefix >
    enhanced_istream_iterator consume_prefix( const enhanced_istream_iterator& begin, const enhanced_istream_iterator& end )
    {
      auto it = begin;
      assert( it != end );
      assert( *it == '0' );
      ++it;
      assert( it != end );
      assert( ( *it == prefix ) || ( *it == alt_prefix ) );
      ++it;
      return it;
    }
  }
}
