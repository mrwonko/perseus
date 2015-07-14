#include "compiler/steps.hpp"
#include "compiler/exceptions.hpp"
#include "shared/optional_apply.hpp"

#include <boost/variant/apply_visitor.hpp>

#include <utility>
#include <tuple>
#include <sstream>

namespace perseus
{
  namespace detail
  {
    namespace clean = perseus::detail::ast::clean;
    namespace parser = perseus::detail::ast::parser;
    namespace art = perseus::detail::ast;

    using namespace std::string_literals;

    /// tag: any type allowed
    struct tag_any_type
    {
    };
    /// tag: any type except type_id::void_ allowed (for variable type deduction)
    struct tag_not_void
    {
    };
    /// Requirements for traversed expressions - must they be pure, according to the function? what type is expected?
    struct expectations
    {
      /// requirements on the type - none, not void, or must be a given type.
      typedef boost::variant<
        tag_any_type,
        tag_not_void,
        // specific type expected
        type_id
      > expected_type;

      expectations( bool pure, expected_type&& type )
        : pure( pure )
        , type( std::move( type ) )
      {
      }

      /// whether the expression must be pure (if false, it may be either)
      bool pure = true;
      expected_type type{ tag_any_type{} };

      bool is_purity_accepted( const bool actual_pure ) const
      {
        return actual_pure || !pure;
      }
      bool is_type_accepted( type_id actual_type ) const
      {
        /// visitor checking whether the type is valid
        struct type_check
        {
          const type_id& actual;
          bool operator()( const tag_any_type& ) const
          {
            return true;
          }
          bool operator()( const tag_not_void& ) const
          {
            return actual != type_id::void_;
          }
          bool operator()( const type_id& expected ) const
          {
            return actual == expected;
          }
        };
        return boost::apply_visitor( type_check{ actual_type }, type );
      }
    };

    struct context
    {
      /// change expectations
      context expect( expectations::expected_type type ) const
      {
        return context{ expectations{ expected.pure, std::move( type ) }, functions, return_type };
      }
      const expectations expected;
      function_manager& functions;
      /// return type of current function, for return expressions
      const type_id return_type;
      // TODO: scope
    };

    /// @throws semantic_error on unknown signature
    static function_manager::function_pointer find_function( const context& context, const function_signature& signature, const file_position& pos )
    {
      function_manager::function_pointer function;
      if( !context.functions.get_function( signature, function ) )
      {
        function_manager::function_map::const_iterator begin, end;
        std::tie( begin, end ) = context.functions.get_functions( signature.name );
        std::stringstream error;
        error << "Could not find " << signature << "!";
        if( begin != end )
        {
          error << " Candidates:";
          while( begin != end )
          {
            error << "\n  " << begin->first;
            ++begin;
          }
        }
        throw semantic_error( error.str(), pos );
      }
      return function;
    }

    static void check_function( const context& context, function_manager::function_pointer function, const file_position& pos )
    {
      if( !context.expected.is_type_accepted( function->second.return_type ) )
      {
        std::stringstream error;
        error << function->first << " returns incorrect type " << get_name( function->second.return_type ) << "!";
        throw type_error{ error.str(), pos };
      }
      if( !context.expected.is_purity_accepted( function->second.pure ) )
      {
        std::stringstream error;
        error << "Can't call impure function " << function->first << " from pure function!";
        throw semantic_error{ error.str(), pos };
      }
    }

    static clean::expression convert( parser::expression&& exp, const context& context );
    static clean::expression convert( parser::operand&& op, const context& context );

    //    Block Member conversion (expression/variable declaration)

    struct convert_block_member
    {
      void check_declaration_valid( const file_position& pos ) const
      {
        if( !context.expected.is_type_accepted( type_id::void_ ) )
        {
          throw semantic_error{ "Variable declaration not valid here!", pos };
        }
      }
      clean::block_member operator()( parser::expression& exp ) const
      {
        return convert( std::move( exp ), context);
      }
      clean::block_member operator()( parser::explicit_variable_declaration& dec ) const
      {
        check_declaration_valid( dec.variable );
        type_id type = get_type( dec.type );
        // declared variable not yet visible in initial value expression
        clean::expression initial_value = convert( std::move( dec.initial_value ), context.expect( type ) );
        // TODO: add variable to scope
        return clean::variable_declaration{ std::move( initial_value ) };
      }
      clean::block_member operator()( parser::deduced_variable_declaration& dec ) const
      {
        check_declaration_valid( dec.variable );
        clean::expression initial_value = convert( std::move( dec.initial_value ), context.expect( tag_not_void{} ) );
        // TODO: add variable to scope
        return clean::variable_declaration{ std::move( initial_value ) };
      }

      const context& context;
    };

    static clean::block_member convert( parser::block_member&& member, const context& context )
    {
      return boost::apply_visitor( convert_block_member{ context }, std::move( member ) );
    }

    //    Operand conversion (constants, variables, control structures etc.)

    struct convert_operand
    {
      //    Constants

      clean::expression operator()( const std::int32_t& i ) const
      {
        // TODO: track position
        if( !context.expected.is_type_accepted( type_id::i32 ) )
        {
          throw type_error{ "i32 type not valid here!", { 0, 0 } };
        }
        return { type_id::i32, { 0, 0 }, i };
      }
      clean::expression operator()( const bool& b ) const
      {
        // TODO: track position
        if( !context.expected.is_type_accepted( type_id::bool_ ) )
        {
          throw type_error{ "bool type not valid here!",{ 0, 0 } };
        }
        return{ type_id::bool_, { 0, 0 }, b };
      }
      clean::expression operator()( const ast::void_expression& ) const
      {
        // TODO: track position
        if( !context.expected.is_type_accepted( type_id::void_ ) )
        {
          throw type_error{ "void type not valid here!",{ 0, 0 } };
        }
        return{ type_id::void_, { 0, 0 }, ast::void_expression{} };
      }
      clean::expression operator()( const ast::string_literal& lit ) const
      {
        // TODO: string support
        throw semantic_error( "Strings not yet supported!"s, lit );
      }

      //    Variable references

      clean::expression operator()( const ast::identifier& identifier ) const
      {
        // TODO: variable lookup
        // TODO: first class functions
        throw std::runtime_error( "variables not yet usable in expressions" );
      }

      //    Unary Operation

      clean::expression operator()( parser::unary_operation& op ) const
      {
        file_position pos = std::move( static_cast< file_position& >( op.operation ) );
        clean::expression operand = convert( std::move( op.operand ), context.expect( tag_not_void{} ) );
        function_signature signature{ std::move( static_cast< std::string& >( op.operation ) ), { operand.type } };
        auto function = find_function( context, signature, pos );
        check_function( context, function, pos );
        return{ function->second.return_type, std::move( pos ), clean::call_expression{ function, { std::move( operand ) } } };
      }

      //    If Expression

      clean::expression operator()( parser::if_expression& exp ) const
      {
        clean::expression condition = convert( std::move( exp.condition ), context.expect( type_id::bool_ ) );
        clean::expression then_expression = convert( std::move( exp.then_expression ), context );
        clean::expression else_expression = convert( std::move( exp.else_expression ), context );
        if( then_expression.type != else_expression.type )
        {
          throw type_error( "\"if\" branch types do not match: then branch returns "s + get_name( then_expression.type ) + ", else branch returns "s + get_name( else_expression.type ) + "!"s, condition.position );
        }
        return{ then_expression.type, condition.position, clean::if_expression{ std::move( condition ), std::move( then_expression ), std::move( else_expression ) } };
      }

      //    While Expression

      clean::expression operator()( parser::while_expression& exp ) const
      {
        clean::expression condition = convert( std::move( exp.condition ), context.expect( type_id::bool_ ) );
        if( !context.expected.is_type_accepted( type_id::void_ ) )
        {
          throw type_error{ "Can't use while expression, void type illegal here!"s, condition.position };
        }
        clean::expression body = convert( std::move( exp.body ), context.expect( tag_any_type{} ) );
        return{ type_id::void_, condition.position, clean::while_expression{ std::move( condition ), std::move( body ) } };
      }

      //    Return Expression

      clean::expression operator()( parser::return_expression& exp ) const
      {
        clean::expression return_value = convert( std::move( exp.value ), context.expect( context.return_type ) );
        return{ context.return_type, return_value.position, clean::return_expression{ std::move( return_value ) } };
      }

      //    Block Expression

      clean::expression operator()( parser::block_expression& block ) const
      {
        std::vector< clean::block_member > members;
        // TODO: track position
        file_position pos{ 0, 0 };
        for( auto it = block.body.begin(); it != block.body.end(); )
        {
          parser::block_member& member = *it;
          bool first = it == block.body.begin();
          ++it;
          // type only matters for last member
          members.emplace_back( convert( std::move( member ), context.expect( it == block.body.end() ? context.expected.type : tag_any_type{} ) ) );
        }
        if( members.empty() )
        {
          if( !context.expected.is_type_accepted( type_id::void_ ) )
          {
            // TODO: track position
            throw type_error( "Block can't be empty, void type illegal here!", pos );
          }
          return{ type_id::void_, pos, clean::block_expression{ std::move( members ) } };
        }
        struct block_type
        {
          type_id operator()( const clean::variable_declaration& ) const
          {
            return type_id::void_;
          }
          type_id operator()( const clean::expression& exp ) const
          {
            return exp.type;
          }
        };
        type_id type = boost::apply_visitor( block_type{}, members.back() );
        return{ type, pos, clean::block_expression{ std::move( members ) } };
      }

      //    Expression

      clean::expression operator()( parser::expression& exp ) const
      {
        return convert( std::move( exp ), context );
      }

      const context& context;
    };

    static clean::expression convert( parser::operand&& op, const context& context )
    {
      return boost::apply_visitor( convert_operand{ context }, std::move( op ) );
    }

    //    Operation conversion

    static clean::expression generate_call( const context& context, const std::string& name, std::vector< clean::expression >&& arguments, file_position position )
    {
      function_signature signature{ std::move( name ),{} };
      signature.parameters.reserve( arguments.size() );
      for( const clean::expression& argument : arguments )
      {
        signature.parameters.push_back( argument.type );
      }
      auto function = find_function( context, signature, position );
      check_function( context, function, position );
      return{ function->second.return_type, std::move( position ), clean::call_expression{ function, std::move( arguments ) } };
    }

    struct convert_operation
    {

      clean::expression operator()( parser::binary_operation& bin )
      {
        return generate_call(
          context,
          // name
          std::move( static_cast< std::string& >( bin.operation ) ),
          // arguments
          { std::move( lhs ), convert( std::move( bin.operand ), context.expect( tag_not_void{} ) ) },
          // position
          std::move( static_cast< file_position& >( bin.operation ) )
          );
      }
      clean::expression operator()( parser::index_expression& exp )
      {
        return generate_call(
          context,
          "[]"s,
          { std::move( lhs ), convert( std::move( exp.index ), context.expect( tag_not_void{} ) ) },
          lhs.position
          );
      }
      clean::expression operator()( parser::call_expression& exp )
      {
        throw std::logic_error( "Should have been handled separately!" );
      }

      clean::expression lhs;
      const context& context;
    };

    static clean::expression convert( parser::expression&& exp, const context& context )
    {
      // TODO FIXME: support more than 2 operands, i.e. handle precedence & associativity
      if( exp.tail.empty() )
      {
        return convert( std::move( exp.head ), context );
      }
      else if( exp.tail.size() == 1 )
      {
        parser::operation& operation = exp.tail.front();
        parser::call_expression* call_exp = boost::get< parser::call_expression >( &operation );
        if( call_exp )
        {
          // since we have no first class functions yet, we need to handle this separately
          ast::identifier* function_name = boost::get< ast::identifier >( &exp.head );
          if( !function_name )
          {
            throw semantic_error{ "Functions must be referred to by identifier! (No first-class functions yet.)"s, {0,0} };
          }
          std::vector< clean::expression > arguments;
          arguments.reserve( call_exp->arguments.size() );
          for( auto& arg : call_exp->arguments )
          {
            arguments.emplace_back( convert( std::move( arg ), context.expect( tag_not_void{} ) ) );
          }
          return generate_call( context, std::move( static_cast< std::string& >( *function_name ) ), std::move( arguments ), std::move( static_cast< file_position& >( *function_name ) ) );
        }
        else
        {
          clean::expression lhs = convert( std::move( exp.head ), context.expect( tag_not_void{} ) );
          return boost::apply_visitor( convert_operation{ std::move( lhs ), context }, std::move( exp.tail.front() ) );
        }
      }
      else
      {
        throw semantic_error{ "Sorry, but for now operator precedence must be explicitly defined using parens."s, { 0, 0 } };
      }
    }

    clean::file clean_parser_ast( parser::file&& file, function_manager& functions )
    {
      ast::clean::file result;
      result.functions.reserve( file.functions.size() );
      for( auto& function : file.functions )
      {
        std::vector< clean::function_argument > args;
        for( parser::function_argument& arg : function.arguments )
        {
          args.emplace_back( clean::function_argument{ std::move( arg.name ), std::move( arg.type ) } );
        }
        // set up context: expected return type, purity
        type_id return_type = function.manager_entry->second.return_type;
        expectations expected{ function.manager_entry->second.pure, return_type };
        context context{ expected, functions, return_type };
        result.functions.emplace_back(
          clean::function_definition{ convert( std::move( function.body ), context ), function.manager_entry }
        );
      }
      return result;
    }
  }
}