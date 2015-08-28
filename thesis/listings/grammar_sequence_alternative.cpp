// syntax tree node definitions
namespace ast
{
  // ...
  struct deduced_variable_declaration
  {
    bool mut;
    identifier variable;
    expression initial_value;
  };
  typedef boost::variant<
    deduced_variable_declaration,
    explicit_variable_declaration,
    expression
  > block_member;
}
// define conversion from tuple to struct
BOOST_FUSION_ADAPT_STRUCT( ast::deduced_variable_declaration,
  mut, variable, initial_value );

// grammar definition (non-terminal symbols)
rule< bool > optional_mutable;
rule< ast::deduced_variable_declaration > deduced_variable_declaration;
rule< ast::block_member > block_member;
// ...

optional_mutable = ( mutable_ >> boost::spirit::qi::attr( true ) )
                 | boost::spirit::qi::attr( false );
deduced_variable_declaration =
  let_ >> optional_mutable >> identifier >> equals >> expression;
block_member = expression
             | explicit_variable_declaration
             | deduced_variable_declaration;