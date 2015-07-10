#pragma once

#include "ast.hpp"

#include <boost/fusion/include/adapt_struct.hpp>

// allow boost.fusion to convert tuples into these objects
// these definitions must be at global scope

BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::parser::expression, head, tail );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::parser::binary_operation, operation, operand );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::parser::unary_operation, operation, operand );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::parser::if_expression, condition, then_expression, else_expression );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::parser::while_expression, condition, body );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::parser::call_expression, arguments );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::parser::block_expression, body );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::parser::explicit_variable_declaration, variable, type, initial_value );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::parser::deduced_variable_declaration, variable, initial_value );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::parser::function_argument, name, type );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::parser::function_definition, name, arguments, type, body );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::parser::file, functions );
