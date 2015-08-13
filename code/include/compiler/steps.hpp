#pragma once

#include "ast.hpp"
#include "function_manager.hpp"

namespace perseus
{
  namespace detail
  {
    class code_segment;

    /**
    @brief Traverses an AST to extract the function declarations

    Annotates the ast::parser::function_definition nodes with the created manager entry.

    This essentially resolves forward references the recursive descent in clean_parser_ast() can't handle.
    @param ast The AST to extract function definitions from; its ast::parser::function_definition nodes get their manager_entry set.
    @param out_functions The function manager handling all the extracted functions
    @throws semantic_error when invalid types are referenced in function signatures
    */
    void extract_functions( ast::parser::file& ast, function_manager& out_functions );
    /**
    @brief Converts parser's AST to one with proper annotated binary operation nodes.

    The @ref grammar "parser" can't contain left recursion, which leads to a fairly ugly AST. That gets post-processed here.
    @param ast the AST generated by the parser
    @returns An equivalent cleaner AST with proper binary operations, instead of n-ary operation-lists.
    */
    ast::clean::file simplify_and_annotate( ast::parser::file&& ast, function_manager& functions );

    void generate_code( const ast::clean::file& ast, code_segment& out_code );
  }
}
