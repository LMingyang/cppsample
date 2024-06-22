#pragma once

#include <string>
#include <variant>

namespace sql {

namespace token {

/**
 * each token is represented by a struct
 */
struct Select {};

struct Identifier {
    std::string name;
};

/**
 * token types
 */
struct From{};

struct Comma{};

struct  Asterisks{};

struct Semicolon{};

} // namespace token


/**
 * class representing a token for the clause
 */
class Token {
public:

    using token_type =
        std::variant<token::Select, token::Identifier, token::From, token::Comma, token::Asterisks, token::Semicolon>;

    Token() = delete;


    explicit Token(token_type value);

    // getter
    [[nodiscard]]
    token_type value() const;

private:
    token_type value_;
};

} // namespace sql
