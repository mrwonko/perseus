// structs shared between ast::parser and ast::clean
// (except they're not quite shared, since the definition of expression varies, so we need to inline them)

/// if-then[-else] expression
struct if_expression
{
  expression condition, then_expression, else_expression;
};

/// while expression
struct while_expression
{
  expression condition, body;
};

struct return_expression
{
#ifdef PERSEUS_AST_PARSER
  return_expression() = default;
  return_expression( const expression& exp )
    : value( exp )
  {
  }
  operator const expression&( ) const
  {
    return value;
  }
#endif
  expression value;
};

struct block_expression
{
  std::vector< block_member > body;
};

struct function_argument
{
  identifier name;
  identifier type;
};

struct function_definition
{
#ifdef PERSEUS_AST_PARSER
  identifier name;
  std::vector< function_argument > arguments;
  boost::optional< identifier > type;
  bool pure;
#endif
  expression body;
#ifdef PERSEUS_AST_PARSER
  // only added during the extract_functions step, initially missing
  boost::optional< function_manager::function_map::const_iterator > manager_entry;
#elif defined PERSEUS_AST_CLEAN
  function_manager::function_map::const_iterator manager_entry;
#endif
};

struct file
{
  std::vector< function_definition > functions;
};
