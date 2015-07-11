// structs shared between ast::parser and ast::clean
// (except they're not quite shared, since the definition of expression varies, so we need to inline them)

/// Unary operation such as -x
struct unary_operation
{
  std::string operation;
  expression operand;
};

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

struct explicit_variable_declaration
{
  identifier variable;
  identifier type;
  expression initial_value;
};

struct deduced_variable_declaration
{
  identifier variable;
  expression initial_value;
};

typedef boost::variant<
  boost::recursive_wrapper< deduced_variable_declaration >,
  boost::recursive_wrapper< explicit_variable_declaration >,
  expression
> block_member;

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
  identifier name;
  std::vector< function_argument > arguments;
  boost::optional< identifier > type;
  expression body;
};

struct file
{
  std::vector< function_definition > functions;
};
