#include <map>
#include <string>
#include <utility>

#include <boost/test/unit_test.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/get.hpp>

// verifying that the compiler implements the standard correctly

BOOST_AUTO_TEST_SUITE( cpp )

using namespace std::string_literals;

BOOST_AUTO_TEST_CASE( map_insertion )
{
  typedef std::map< std::string, std::string > map;
  map m;
  {
    map::const_iterator a = m.emplace( std::make_pair( "a"s, "a value"s ) ).first;
    // this should not invalidate previous iterators
    map::const_iterator b = m.emplace( std::make_pair( "b"s, "b value"s ) ).first;
    BOOST_CHECK_EQUAL( a->first, "a"s );
    BOOST_CHECK_EQUAL( a->second, "a value"s );
    BOOST_CHECK_EQUAL( b->first, "b"s );
    BOOST_CHECK_EQUAL( b->second, "b value"s );
  }
  BOOST_CHECK_EQUAL( m[ "a"s ], "a value"s );
  BOOST_CHECK_EQUAL( m[ "b"s ], "b value"s );
}

BOOST_AUTO_TEST_CASE( iterator_move )
{
  typedef std::map< std::string, std::string > map;
  map m;
  {
    map::const_iterator a = m.emplace( std::make_pair( "a"s, "a value"s ) ).first;
    map::const_iterator b = std::move( a );
    BOOST_CHECK_EQUAL( b->first, "a"s );
    BOOST_CHECK_EQUAL( b->second, "a value"s );
  }
  BOOST_CHECK_EQUAL( m[ "a"s ], "a value"s );
}

BOOST_AUTO_TEST_CASE( variant_move )
{
  typedef std::map< std::string, std::string > map;
  struct inner_node;
  struct leaf
  {
    map::const_iterator it;
  };
  typedef boost::variant<
    leaf,
    boost::recursive_wrapper< inner_node >
  > node;
  struct inner_node
  {
    node child;
  };

  map m;
  map::const_iterator it = m.emplace( std::make_pair( "a"s, "a value"s ) ).first;
  node root = inner_node{ leaf{ it } };
  node root2 = std::move( root );
  inner_node* n = boost::get< inner_node >( &root2 );
  BOOST_REQUIRE_NE( n, static_cast< inner_node* >( nullptr ) );
  leaf* l = boost::get< leaf >( &n->child );
  BOOST_REQUIRE_NE( l, static_cast< leaf* >( nullptr ) );
  BOOST_CHECK( l->it == it );
}

BOOST_AUTO_TEST_SUITE_END()
