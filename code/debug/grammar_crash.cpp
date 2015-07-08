#include <boost/spirit/home/qi.hpp>

#include <iostream>
#include <string>

struct default_type
{
};

int main( int, char** )
{
  namespace qi = boost::spirit::qi;

  using result_type = boost::variant< int, default_type >;

  qi::rule< std::string::const_iterator, result_type() > grammar = qi::int_ | qi::attr( default_type() );

  std::string source = "42";
  result_type result;
  std::cout << std::boolalpha << qi::parse( source.begin(), source.end(), grammar, result ) << std::endl;

  return 0;
}
