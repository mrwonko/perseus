namespace ast
{
  //...
  struct expression
  {
    operand head;
    std::vector< operation > tail;
  };
}

rule< ast::expression > expression;
//...

expression = operand >> *operation;