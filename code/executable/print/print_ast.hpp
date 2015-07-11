#pragma once

#include <boost/variant/static_visitor.hpp>

#include <iostream>
#include <string>

class print_visitor : public boost::static_visitor<>
{
protected:
  static constexpr unsigned int INDENT_SPACES = 2;

public:
  print_visitor()
    : _indent( 0 )
  {
  }

  print_visitor( unsigned int indent )
    : _indent( indent )
    , _indent_string( indent, ' ' )
  {
  }

protected:

  std::ostream& indent() const
  {
    return std::cout << _indent_string << "* ";
  }

  const unsigned int _indent;
  const std::string _indent_string;
};
