#include "compiler/steps.hpp"
#include "vm/code_segment.hpp"

#include <boost/variant/apply_visitor.hpp>

#include <cassert>
#include <cstdint>
#include <string>
#include <stdexcept>

namespace perseus
{
  namespace detail
  {
    using namespace std::string_literals;

    struct context
    {
      void push( const std::uint32_t bytes )
      {
        stack_size += bytes;
      }

      void pop( const std::uint32_t bytes )
      {
        assert( bytes <= stack_size );
        stack_size -= bytes;
      }

      const function_manager::function_pointer& function;
      /// counting from above the return address of the current function
      std::uint32_t stack_size;
      code_segment& code;
    };

    template< typename T >
    struct value_placeholder
    {
      instruction_pointer::value_type address;
    };

    template< typename T >
    static value_placeholder< T > write_placeholder( code_segment& code )
    {
      value_placeholder< T > placeholder{ code.size() };
      code.push< T >( 0 );
      return placeholder;
    }

    template< typename T >
    static void replace_placeholder( code_segment& code, value_placeholder< T > placeholder, T value )
    {
      code.write< T >( placeholder.address, value );
    }

    static void generate_code( const ast::clean::expression& expression, context& c );

    struct generate_expression_code
    {
      //    Constants

      void operator()( const ast::void_expression& ) const
      {
        // no-op
      }
      void operator()( const ast::string_literal& ) const
      {
        throw std::logic_error{ "Not yet implemented, but such a tree should not have been created?!"s };
      }
      void operator()( const std::int32_t& constant ) const
      {
        c.code.push< opcode >( opcode::push_32 );
        c.code.push< std::int32_t >( constant );
        c.push( sizeof( std::int32_t ) );
      }
      void operator()( const bool& constant ) const
      {
        c.code.push< opcode >( opcode::push_8 );
        c.code.push< std::uint8_t >( constant );
        c.push( sizeof( std::uint8_t ) );
      }

      //    Variable Reference

      void operator()( const ast::clean::local_variable_reference& ) const
      {
        throw std::logic_error{ "Variable usage not yet implemented, but such a tree should not have been created?!"s };
      }

      //    If Condition

      void operator()( const ast::clean::if_expression& exp ) const
      {
        assert( exp.condition.type == type_id::bool_ );
        assert( exp.then_expression.type == exp.else_expression.type );

        //    condition
        // evaluate condition
        generate_code( exp.condition, c );
        // conditional jump
        c.code.push< opcode >( opcode::relative_jump_if_false );
        auto else_offset_placeholder = write_placeholder< int32_t >( c.code );
        auto else_jump_address = c.code.size();
        c.pop( 1 );

        //    then expression
        auto pre_then_stack_size = c.stack_size;
        generate_code( exp.then_expression, c );
        // jump past else
        c.code.push< opcode >( opcode::relative_jump );
        auto end_offset_placeholder = write_placeholder< int32_t >( c.code );
        auto end_jump_address = c.code.size();

        //    else expression
        // else label
        replace_placeholder< int32_t >( c.code, else_offset_placeholder, static_cast< int32_t >( c.code.size() - else_jump_address ) );
        c.stack_size = pre_then_stack_size; // due to the jump, the then result is not pushed onto the stack at this point
        generate_code( exp.else_expression, c );

        //    past-else label
        replace_placeholder< int32_t >( c.code, end_offset_placeholder, static_cast< int32_t >( c.code.size() - end_jump_address ) );
      }

      //    While Loop

      void operator()( const ast::clean::while_expression& exp ) const
      {
        assert( exp.condition.type == type_id::bool_ );

        auto start_address = c.code.size();
        //    loop condition
        generate_code( exp.condition, c );
        // conditional jump
        c.code.push< opcode >( opcode::relative_jump_if_false );
        auto end_offset_placeholder = write_placeholder< int32_t >( c.code );
        auto end_jump_address = c.code.size();
        c.pop( 1 );

        //    loop body
        generate_code( exp.body, c );
        // discard result
        auto body_result_size = get_size( exp.body.type );
        if( body_result_size > 0 )
        {
          c.code.push< opcode >( opcode::pop );
          c.code.push< std::uint32_t >( body_result_size );
          c.pop( body_result_size );
        }
        // jump back to condition
        c.code.push< opcode >( opcode::relative_jump );
        // the extra int32 size is of the address that's being pushed
        c.code.push< std::int32_t >( -static_cast< std::int32_t >( c.code.size() + sizeof( std::int32_t ) - start_address ) );

        // end label
        replace_placeholder< int32_t >( c.code, end_offset_placeholder, static_cast< int32_t >( c.code.size() - end_jump_address ) );
      }

      //    Return

      void operator()( const ast::clean::return_expression& exp ) const
      {
        // calculate return value
        generate_code( exp.value, c );

        // store return value
        auto return_size = get_size( exp.value.type );
        auto argument_size = c.function->first.parameters_size();
        if( return_size > 0 )
        {
          c.code.push< opcode >( opcode::relative_store_stack );
          c.code.push< std::uint32_t >( return_size );
          c.code.push< std::int32_t >( -static_cast< int32_t >( return_size + argument_size + sizeof( instruction_pointer::value_type ) + c.stack_size ) );
        }

        // pop everything
        if( c.stack_size > 0 )
        {
          c.code.push< opcode >( opcode::pop );
          c.code.push< std::uint32_t >( c.stack_size );
        }
        // return
        c.code.push< opcode >( opcode::return_ );
        c.code.push< std::uint32_t >( argument_size );
      }

      //    Block

      struct generate_block_member_code
      {
        void operator()( const ast::clean::expression& exp ) const
        {
          auto pre_expression_stack_size = c.stack_size;
          generate_code( exp, c );
          auto return_size = c.stack_size - pre_expression_stack_size;
          if( last )
          {
            // last expression in block is returned (i.e. kept on stack).
            // are there local variables?
            if( pre_expression_stack_size != pre_block_stack_size )
            {
              auto local_variables_size = pre_expression_stack_size - pre_block_stack_size;
              // store result
              c.code.push< opcode >( opcode::relative_store_stack );
              c.code.push< std::uint32_t >( return_size );
              c.code.push< std::int32_t >( -static_cast< std::int32_t >( local_variables_size + return_size ) );
              // clean up local variables
              c.code.push< opcode >( opcode::pop );
              c.code.push< std::uint32_t >( local_variables_size );
              c.pop( local_variables_size );
            }
          }
          else
          {
            // discard result
            if( return_size != 0 )
            {
              assert( c.stack_size > pre_expression_stack_size );
              c.code.push< opcode >( opcode::pop );
              c.code.push< std::uint32_t >( return_size );
              c.stack_size = pre_expression_stack_size;
            }
          }
        }

        void operator()( const ast::clean::variable_declaration& dec ) const
        {
          generate_code( dec.initial_value, c );
          if( last )
          {
            // clean up local variables
            if( c.stack_size != pre_block_stack_size )
            {
              assert( c.stack_size > pre_block_stack_size );
              c.code.push< opcode >( opcode::pop );
              c.code.push< std::uint32_t >( c.stack_size - pre_block_stack_size );
              c.stack_size = pre_block_stack_size;
            }
          }
        }

        const bool last;
        std::uint32_t pre_block_stack_size;
        context& c;
      };

      void operator()( const ast::clean::block_expression& block ) const
      {
        auto pre_block_stack_size = c.stack_size;
        for( auto it = block.body.begin(); it != block.body.end(); )
        {
          const ast::clean::block_member& member = *it;
          ++it;
          const bool last = it == block.body.end();

          auto pre_member_stack_size = c.stack_size;
          boost::apply_visitor( generate_block_member_code{ last, pre_block_stack_size, c }, member );
        }
      }

      //    Call

      void operator()( const ast::clean::call_expression& exp ) const
      {
        //    Reserve result space (not for opcodes though)
        auto return_size = get_size( exp.function->second.return_type );
        if( return_size > 0 && !exp.function->second.has_opcode() )
        {
          c.code.push< opcode >( opcode::reserve );
          c.code.push< std::uint32_t >( return_size );
          c.push( return_size );
        }
        auto post_call_stack_size = c.stack_size;

        //    Push arguments (left to right)
        for( const ast::clean::expression& arg : exp.arguments )
        {
          generate_code( arg, c );
        }
        opcode op;
        if( exp.function->second.get_opcode( op ) )
        {
          c.code.push< opcode >( op );
          c.stack_size = post_call_stack_size + return_size;
        }
        else
        {
          c.code.push< opcode >( opcode::call );
          exp.function->second.write_address( c.code );
          c.stack_size = post_call_stack_size;
        }
      }

      context& c;
    };

    static void generate_code( const ast::clean::expression& expression, context& c )
    {
      boost::apply_visitor( generate_expression_code{ c }, expression.subexpression );
    }

    static void generate_code( const ast::clean::function_definition& function, context& c )
    {
      // we now know this functions' address; make it known
      c.function->second.set_address( c.code.size(), c.code );

      const type_id return_type = c.function->second.return_type;
      assert( function.body.type == return_type );

      // calculate result
      generate_code( function.body, c );
      assert( c.stack_size == get_size( return_type ) );

      // store result, if any
      auto return_size = get_size( return_type );
      auto argument_size = c.function->first.parameters_size();
      if( return_size > 0 )
      {
        // stack: <return memory> <parameters> <return address> <result>
        c.code.push< opcode >( opcode::relative_store_stack );
        c.code.push< std::uint32_t >( return_size );
        c.code.push< std::int32_t >( -static_cast< std::int32_t >( 2 * return_size + argument_size + sizeof( instruction_pointer::value_type ) ) );
        // stack: <result> <parameters> <return address> <result>
        c.code.push< opcode >( opcode::pop );
        c.code.push< std::uint32_t>( get_size( return_type ) );
        // stack: <result> <parameters> <return address>
      }
      c.code.push< opcode >( opcode::return_ );
      c.code.push< std::uint32_t >( argument_size );
      // stack: <result>
    }

    void generate_code( const ast::clean::file& ast, code_segment& out_code )
    {
      for( const ast::clean::function_definition& function : ast.functions )
      {
        context c{ function.manager_entry, 0u, out_code };
        generate_code( function, c );
      }
    }
  }
}
