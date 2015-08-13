#include "compiler/compiler.hpp"
#include "compiler/token_definitions.hpp"
#include "compiler/exceptions.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <boost/spirit/home/qi/operator/expect.hpp>

using namespace std::string_literals;

void print_usage( const char* program )
{
  std::cout << "Usage: " << program << " (-e <code>|<filename>)*" << std::endl;
  std::cout << "Your program must have a main() function." << std::endl;
}

int main( int argc, const char** argv )
{
  if( argc == 1 )
  {
    print_usage( argv[ 0 ] );
    return 0;
  }
  try
  {
    perseus::compiler compiler;
    int i = 1;
    while( i < argc )
    {
      if( argv[ i ] == "-e"s )
      {
        if( ++i == argc )
        {
          print_usage( argv[ 0 ] );
          return 1;
        }
        std::stringstream code( argv[ i ] );
        compiler.parse( code, "<argument"s + std::to_string( i ) + " code>"s );
      }
      else
      {
        std::ifstream file( argv[ i ] );
        if( !file )
        {
          std::cerr << "Failed to open file " << argv[ i ] << "!" << std::endl;
          return 1;
        }
        compiler.parse( file, argv[ i ] );
      }
      std::cout << "Compiled " << argv[ i ] << std::endl;
      i += 1;
    }
    auto processor = compiler.link();
    std::cout << "Linked program." << std::endl;
    auto result_stack = processor.execute();
    std::cout << "Result stack:";
    for( auto byte : result_stack )
    {
      std::cout << " 0x" << std::setfill( '0' ) << std::setw( 2 ) << std::hex << static_cast< unsigned int >( byte );
    }
    std::cout << std::endl;
  }
  catch( boost::spirit::qi::expectation_failure< perseus::detail::token_iterator >& e )
  {
    // FIXME: the dereferencing presumably fails for empty inputs
    std::cerr << "Expectation failure: " << e.first->id() << " at " << e.first->value().begin().get_position() << std::endl;
  }
  catch( perseus::compile_error& e )
  {
    std::cerr << "Compile error at " << e.location << ": " << e.what() << std::endl;
  }
  catch( std::exception& e )
  {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
}
