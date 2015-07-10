#pragma once

#include "ast.hpp"

#include <boost/fusion/include/adapt_struct.hpp>

// allow boost.fusion to convert tuples into these objects
// these definitions must be at global scope

BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::expression, head, tail );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::binary_operation, operation, operand );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::unary_operation, operation, operand );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::if_expression, condition, then_expression, else_expression );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::while_expression, condition, body );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::call_expression, arguments );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::block_expression, body );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::explicit_variable_declaration, variable, type, initial_value );
BOOST_FUSION_ADAPT_STRUCT( perseus::detail::ast::deduced_variable_declaration, variable, initial_value );
