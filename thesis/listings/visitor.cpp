struct convert_block_member
{
  transformed_ast::block_member operator()( ast::expression& exp ) const
  { /* ... */ }
  transformed_ast::block_member operator()( ast::explicit_variable_declaration& dec ) const
  { /* ... */ }
  transformed_ast::block_member operator()( ast::deduced_variable_declaration& dec ) const
  { /* ... */ }
  context& context;
};

transformed_ast::block_member convert( ast::block_member&& member, context& context )
{
  return boost::apply_visitor( convert_block_member{ context }, std::move( member ) );
}