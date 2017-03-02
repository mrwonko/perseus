#pragma once

#include "ast.hpp"
#include "token_definitions.hpp"
#include "compiler/exceptions.hpp"

#include <boost/spirit/home/qi/parser.hpp>
#include <boost/spirit/home/support/context.hpp>
#include <boost/spirit/home/qi/skip_over.hpp>

#include <cassert>

namespace perseus
{
  namespace detail
  {
    using namespace std::string_literals;

    /**
    read one code point from a utf8 sequence
    @throws invalid_utf8 on stray or missing continuations
    */
    char32_t readCodePoint( enhanced_istream_iterator& it, const enhanced_istream_iterator& end );

    struct string_literal_parser : boost::spirit::qi::primitive_parser< string_literal_parser >
    {
      /// meta-information about the resulting attribute
      template< typename Context, typename Iterator >
      struct attribute
      {
        /// this parser yields an integer
        typedef ast::string_literal type;
      };

      /**
      This function is called when the parser tries to match a string literal. It returns if a string literal was matched and moves the iterator past it if it did.
      */
      template< typename Context, typename Skipper, typename Attribute >
      bool parse( token_iterator& token_it, const token_iterator& token_end, Context&, const Skipper& skipper, Attribute& out_attribute ) const
      {
        boost::spirit::qi::skip_over( token_it, token_end, skipper );
        if( token_it == token_end )
        {
          return false;
        }
        const token& t = *token_it;
        if( t.id() != token_id::string )
        {
          return false;
        }

        // this is supposed to be a string literal, so let's try parsing it.
        const enhanced_istream_iterator& begin = t.value().begin(), end = t.value().end();
        // due to the regex in the lexer we can assume the string to be wrapped in quotes
        assert( std::distance( begin, end ) >= 2 );
        assert( *begin == '"' );
        out_attribute.clear();
        // skip opening quote
        enhanced_istream_iterator it = begin;
        ++it;
        for( ; it != end; ++it )
        {
          char32_t c = readCodePoint( it, end );
          if( c != '\\' )
          {
            out_attribute.push_back( c );
          }
          else
          {
            // unescape character
            switch( readCodePoint( it, end ) )
            {
            case U'\\':
              out_attribute.push_back( U'\\' );
              break;
            case U'"':
              out_attribute.push_back( U'"' );
              break;
            case U'n':
              out_attribute.push_back( U'\n' );
              break;
            case U'r':
              out_attribute.push_back( U'\r' );
              break;
            case U't':
              out_attribute.push_back( U'\t' );
              break;
            default:
              // the lexer should not give us invalid escape codes, as per the regular expression
              assert( false );
            }
          }
        }
        // remove trailing quote (ForwardIterator means we can't stop one before end)
        assert( !out_attribute.empty() );
        assert( out_attribute.back() == U'"' );
        out_attribute.pop_back();

        ++token_it;
        return true;
      }

      template< typename Context >
      boost::spirit::qi::info what( Context& )
      {
        return boost::spirit::qi::info{ "string literal" };
      }
    };

    struct parsed_string_literal : ast::string_literal
    {
      parsed_string_literal() = default;
      parsed_string_literal( const enhanced_istream_iterator& begin, const enhanced_istream_iterator& end );
    };

    template< typename T, T base >
    T parse_integer_literal( const enhanced_istream_iterator& begin, const enhanced_istream_iterator& end, enhanced_istream_iterator it );

    template< char prefix, char alt_prefix >
    enhanced_istream_iterator consume_prefix( const enhanced_istream_iterator& begin, const enhanced_istream_iterator& end );

    // TODO: sign prefix? as it stands, the we overflow on the smallest number since the - is parsed as an unary operator
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
        out_attribute = parse_integer_literal< T, base >(
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
        return boost::spirit::qi::info{ "integer literal" };
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
