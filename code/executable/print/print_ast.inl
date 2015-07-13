void operator()( const ast::file& file ) const
{
  indent() << "<file>" << std::endl;
  for( const ast::function_definition& func : file.functions )
  {
    recurse( func );
  }
}

void operator()( const ast::function_argument& arg )
{
  indent()
    << "<function argument "
    << static_cast< const std::string& >( arg.name )
    << ": "
    << static_cast< const std::string& >( arg.type )
    << std::endl;
}

void operator()( const ast::void_expression& ) const
{
  indent() << "<void expression>" << std::endl;
}

void operator()( const ast::string_literal& str ) const
{
  indent() << "<string literal \"" << static_cast< const std::u32string& >( str ) << "\">" << std::endl;
}

void operator()( std::int32_t n ) const
{
  indent() << "<integer literal " << n << ">" << std::endl;
}

void operator()( bool b ) const
{
  indent() << "<bool literal " << std::boolalpha << b << ">" << std::endl;
}

void operator()( const ast::if_expression& exp ) const
{
  indent() << "<if>" << std::endl;
  recurse( exp.condition );
  recurse( exp.then_expression );
  recurse( exp.else_expression );
}

void operator()( const ast::while_expression& exp ) const
{
  indent() << "<while>" << std::endl;
  recurse( exp.condition );
  recurse( exp.body );
}

void operator()( const ast::return_expression& exp ) const
{
  indent() << "<return>" << std::endl;
  recurse( exp.value );
}

void operator()( const ast::block_expression& block ) const
{
  indent() << "<block>" << std::endl;
  for( const ast::block_member& member : block.body )
  {
    recurse( member );
  }
}
