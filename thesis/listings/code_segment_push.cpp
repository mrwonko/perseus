template< typename T >
void push( const T& value )
{
  static_assert( std::is_trivially_copyable< T >::value, "code_segment data must be trivially copyable!" );
  const_pointer begin = reinterpret_cast< const_pointer >( &value );
  const_pointer end = begin + sizeof( T );
  insert( std::vector< char >::end(), begin, end );
}

// specialization for opcodes; encodes them
template<>
void push< opcode >( const opcode& code )
{
  push_opcode( code );
}