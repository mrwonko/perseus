#pragma once

#include <boost/spirit/home/qi/nonterminal/rule.hpp>
#include <boost/spirit/home/qi/nonterminal/grammar.hpp>

#include "ast.hpp"
#include "token_definitions.hpp"

namespace perseus
{
  namespace detail
  {
    /// Tokens to skip during parse; takes care of ignoring comments and whitespace
    struct skip_grammar : boost::spirit::qi::grammar< token_iterator >
    {
      /// constructor defining the grammar
      skip_grammar();
    };

    /**
    @brief Syntax Definition of the Perseus Language
    */
    struct grammar : boost::spirit::qi::grammar< token_iterator, ast::parser::file(), skip_grammar >
    {
      /// constructor defining the grammar
      grammar();
    };
  }
}
