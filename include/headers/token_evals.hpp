#ifndef TOKEN_EVALS_HPP
#define TOKEN_EVALS_HPP

#include "headers/types.hpp"
#include "headers/token.hpp"

namespace stone_lang {

Generic type_eval(Token* token);
Generic name_eval(Token* token);
Generic scope_eval(Token* token);
Generic expression_eval(Token* token);
Generic operator_eval(Token* token);
Generic literal_eval(Token* token);

} // namespace stone_lang

#endif