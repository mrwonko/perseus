#pragma once

#include "ast.hpp"
#include "type_manager.hpp"
#include "function_manager.hpp"

namespace perseus
{
  namespace detail
  {
    /**
    @brief Traverses an AST to extract the type definitions
    @returns A type manager handling all the extracted types as well as the base ones
    @todo Implement user-defined types; this is pretty much a no-op at the moment
    */
    type_manager extract_types( ast::parser::file& ast );
    /**
    @brief Traverses an AST to extract the function declarations
    @returns A function manager handling all the extracted functions
    @throws semantic_error when invalid types are referenced in function signatures
    */
    function_manager extract_functions( const ast::parser::file& ast, const type_manager& types );
    /**
    @brief Converts AST to one with proper binary operation nodes.

    The @ref grammar "parser" can't contain left recursion, which leads to a fairly ugly AST. That gets post-processed here.
    @param ast the AST generated by the parser
    @returns An equivalent cleaner AST with proper binary operations, instead of n-ary operation-lists.
    */
    ast::clean::file clean_parser_ast( ast::parser::file&& ast, const type_manager& types, const function_manager& functions );
  }
}
