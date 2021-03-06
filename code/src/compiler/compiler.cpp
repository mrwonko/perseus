#include "compiler/compiler.hpp"

#include "compiler/grammar.hpp"
#include "compiler/exceptions.hpp"
#include "compiler/ast.hpp"
#include "compiler/function_manager.hpp"
#include "compiler/steps.hpp"
#include "shared/opcodes.hpp"

#include <boost/spirit/home/qi/parse.hpp>

#include <vector>
#include <utility>
#include <stdexcept>
#include <iostream>

namespace perseus
{
  using namespace std::string_literals;

  struct compiler::impl
  {
    detail::token_definitions lexer;
    detail::grammar parser;
    detail::skip_grammar skipper;
    bool linking = false;
    std::vector< detail::ast::parser::file > files;
  };

  compiler::compiler()
    : _impl( std::make_unique< compiler::impl >() )
  {
  }
  // these are in the .cpp since they require with compiler::impl, which is only forward declared in the .hpp
  compiler::~compiler() = default;
  compiler::compiler( compiler&& ) = default;
  compiler& compiler::operator=( compiler&& ) = default;

  void compiler::reset()
  {
    // keep lexer, parser & skipper intact - the entire point of the compiler class is keeping their results cached
    _impl->linking = false;
    _impl->files.clear();
  }

  void compiler::parse( std::istream& source_stream, const std::string& filename )
  {
    detail::enhanced_istream_iterator input_it, input_end;
    // TODO: add filename to file_position and track it
    std::tie( input_it, input_end ) = detail::enhanced_iterators( source_stream );
    detail::skip_byte_order_mark( input_it, input_end );

    // I suppose unless I need tokens_it/tokens_end, I could just lex::tokenize_and_phrase_parse, but doing it manually gives me more options
    detail::token_iterator tokens_it = _impl->lexer.begin( input_it, input_end );
    const detail::token_iterator tokens_end = _impl->lexer.end();

    detail::ast::parser::file result;
    bool success = boost::spirit::qi::phrase_parse( tokens_it, tokens_end, _impl->parser, _impl->skipper, result );

    if( !success )
    {
      // TODO: throw better error
      throw syntax_error( 
        "Failed to parse " + filename, 
        tokens_it == tokens_end ? detail::file_position() : tokens_it->value().begin().get_position()
        );
    }
    // no need to check if it == end, since the grammar contains an eoi parser.
    _impl->files.emplace_back( std::move( result ) );
  }

  detail::processor compiler::link()
  {
    if( _impl->linking )
    {
      throw std::logic_error{ "Called compiler::link() again after failure without calling compiler::reset() first!" };
    }
    _impl->linking = true; // set it now in case of exceptions since we don't roll back on failure

    detail::function_manager function_manager;

    // register builtin functions
    detail::function_manager::function_pointer print_i32_function, print_bool_function;
    function_manager.register_function( detail::function_signature{ "print"s, { detail::type_id::i32 } }, detail::function_info{ detail::type_id::void_, false }, print_i32_function );
    function_manager.register_function( detail::function_signature{ "print"s, { detail::type_id::bool_ } }, detail::function_info{ detail::type_id::void_, false }, print_bool_function );

    // collect available functions
    for( auto& file_ast : _impl->files )
    {
      detail::extract_functions( file_ast, function_manager );
    }

    //        generate code

    //    entry point

    // find main function
    detail::code_segment code;
    detail::function_manager::function_pointer main_function;
    if( !function_manager.get_function( { "main"s, {} }, main_function ) )
    {
      throw semantic_error{ "no main() function (without parameters) defined!"s, { 0, 0 } };
    }
    // reserve return value space
    {
      auto size = get_size( main_function->second.return_type );
      if( size > 0 )
      {
        code.push< detail::opcode >( detail::opcode::reserve );
        code.push< std::uint32_t >( size );
      }
    }
    // call main function
    code.push< detail::opcode >( detail::opcode::call );
    main_function->second.write_address( code );
    // exit
    code.push< detail::opcode >( detail::opcode::exit );

    //    generate builtin functions
    std::vector< detail::syscall > syscalls;
    auto generate_builtin_code = [ &syscalls, &code ]( detail::function_manager::function_pointer function, detail::syscall&& definition )
    {
      function->second.set_address( code.size(), code );
      code.push< detail::opcode >( detail::opcode::syscall );
      code.push< std::uint32_t >( syscalls.size() );
      code.push< detail::opcode >( detail::opcode::return_ );
      code.push< std::uint32_t >( function->first.parameters_size() );
      syscalls.emplace_back( std::move( definition ) );
    };

    // print i32
    generate_builtin_code( print_i32_function, []( stack& s ) {
      const char* memory = s.data() + s.size() - sizeof( detail::instruction_pointer::value_type ) - detail::get_size( detail::type_id::i32 );
      std::cout << *reinterpret_cast< const int* >( memory ) << std::endl;
    } );
    // print bool
    generate_builtin_code( print_bool_function, []( stack& s ) {
      const char* memory = s.data() + s.size() - sizeof( detail::instruction_pointer::value_type ) - detail::get_size( detail::type_id::bool_ );
      bool value = *memory;
      std::cout << std::boolalpha << value << std::endl;
    } );

    //    append functions
    while( !_impl->files.empty() )
    {
      generate_code( detail::simplify_and_annotate( std::move( _impl->files.back() ), function_manager ), code );
      _impl->files.pop_back();
    }
    if( function_manager.has_open_address_requests() )
    {
      throw std::logic_error{ "Unhandled address requests?!" };
    }
    _impl->linking = false;
    return detail::processor{ std::move( code ), std::move( syscalls ) };
  }

}
