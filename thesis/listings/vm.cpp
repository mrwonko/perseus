struct coroutine
{
  instruction_pointer instruction_pointer;
  stack stack;
  // ...
}

coroutine& co = /* ... */;
instruction_pointer& ip = co.instruction_pointer;
while( true )
{
  switch( ip.read< opcode >() )
  {
    case opcode::push_8:
      co.stack.push< char >( ip.read< char >() );
      break;
    // ...
    case opcode::absolute_jump:
      ip = ip.read< std::uint32_t >();
      break;
    case opcode::relative_jump_if_false:
    {
      std::int32_t offset = ip.read< std::int32_t >();
      if( !co.stack.pop< std::uint8_t >() )
      {
        ip += offset;
      }
      break;
    }
    case opcode::call:
    {
      const std::uint32_t target_address = ip.read< std::uint32_t >();
      co.stack.push< std::uint32_t >( ip.value() );
      ip = target_address;
      break;
    }
    // ...
    case opcode::add_i32:
    {
      const std::int32_t op2 = co.stack.pop< std::int32_t >(), op1 = co.stack.pop< std::int32_t >();
      co.stack.push< std::int32_t >( op1 + op2 );
      break;
    }
    // ...
  }
}