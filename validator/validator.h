#pragma once

#include <variant>
#include <vector>

#include "token.h"

namespace sql {

namespace state {

struct Start {};

/**
 * if any transition was wrong, the FSM will stay in the invalid state no matter further tokens
 */
struct Invalid {};

/**
 * if the sequence of tokens is correct, the FSM is in the valid state
 */
struct Valid {};

struct SelectStmt{};

struct AllColumns{};

struct NamedColumn{};

struct MoreColumns{};

struct FromClause{};

struct TableName{};

} // namespace state


/**
 * variant of all possible states
 */
using State =
    std::variant<state::Start, state::Invalid, state::Valid, state::SelectStmt, state::AllColumns, state::NamedColumn, state::MoreColumns,
                    state::FromClause, state::TableName>;


[[nodiscard]]
State transition(state::Start, const Token &token);

[[nodiscard]]
State transition(state::Valid, const Token &token);

[[nodiscard]]
State transition(state::Invalid, const Token &token);

[[nodiscard]]
State transition(state::SelectStmt, const Token &token);

[[nodiscard]]
State transition(state::AllColumns, const Token &token);

[[nodiscard]]
State transition(state::NamedColumn, const Token &token);

[[nodiscard]]
State transition(state::MoreColumns, const Token &token);

[[nodiscard]]
State transition(state::FromClause, const Token &token);

[[nodiscard]]
State transition(state::TableName, const Token &token);

/**
 * the initial state is `Start`, moves to the next state based on the given tokens
 * query with`is_valid`
 */
class SqlValidator {
public:
    SqlValidator() = default;

    [[nodiscard]]
    bool is_valid() const;

/**
 * moves from one state to the next
 */
    void handle(const Token &token);

private:
    State state_ = state::Start{};
};


/**
 * return true if a sequence of tokens is valid
 */
[[nodiscard]]
bool is_valid_sql_query(const std::vector<Token> &tokens);
} // namespace sql
