#include "validator.h"

#include <variant>
#include <vector>

#include "token.h"

namespace sql {

bool SqlValidator::is_valid() const {

    return std::holds_alternative<state::Valid>(state_);
}

void SqlValidator::handle(const Token &token) {

    state_ = std::visit([&](auto cur) -> State {
        return transition(cur,token);
    }, state_);

    /*auto lambda = [&](auto cur) -> State {
        return transition(cur,token);
    };
    state_ = std::visit(lambda,state_);*/
}

struct TransitionFromStartVisitor {
    // only the `Select` token advances the FSM from the `Start` state
    State operator()(token::Select) const { return state::SelectStmt{}; }

    State operator()(auto) const { return state::Invalid{}; }
};

State transition(state::Start, const Token &token) {
    return std::visit(TransitionFromStartVisitor{}, token.value());
}

State transition(state::Valid, const Token &token) {
    if(token.value().index() == 5)
        return state::Valid{};
    else
        return state::Invalid{};
}

State transition(state::Invalid, const Token &token) {
    return state::Invalid{};
}

struct TransitionSelectStmtVisitor {
    State operator() (token::Asterisks) const { return state::AllColumns{}; }
    State operator() (token::Identifier) const { return state::NamedColumn{}; }
    State operator() (auto) const { return state::Invalid{}; }
};

State transition(state::SelectStmt, const Token &token){
    return std::visit(TransitionSelectStmtVisitor{}, token.value());
}

State transition(state::AllColumns, const Token &token){
    if(token.value().index() == 2)
        return state::FromClause{};
    else
        return state::Invalid{};
}

struct TransitionNamedColumnVisitor{
    State operator() (token::From) const { return state::FromClause{}; }
    State operator() (token::Comma) const { return state::MoreColumns{}; }
    State operator() (auto) const { return state::Invalid{}; }
};
State transition(state::NamedColumn, const Token &token){
    return std::visit(TransitionNamedColumnVisitor{}, token.value());
}

State transition(state::MoreColumns, const Token &token){
    if(token.value().index() == 1)
        return state::NamedColumn{};
    else
        return state::Invalid{};
}

State transition(state::FromClause, const Token &token){
    if(token.value().index() == 1)
        return state::TableName{};
    else
        return state::Invalid{};
}

State transition(state::TableName, const Token &token){
    if(token.value().index() == 5)
        return state::Valid{};
    else
        return state::Invalid{};
}

[[nodiscard]]
bool is_valid_sql_query(const std::vector<Token> &tokens){
    SqlValidator sqlValidator;
    for(const auto& item : tokens){
        sqlValidator.handle(item);
    }
    return sqlValidator.is_valid();
}
} // namespace sql
