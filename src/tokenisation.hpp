#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <optional>

using namespace std;

enum class TokenType {
    _exit,
    int_lit,
    semi,
    open_paren,
    close_paren,
    ident,
    var,
    eq,
    plus,
    star
};

optional<int> bin_prec(TokenType type) {
    switch (type) {
        case TokenType::plus:
            return 0;
        case TokenType::star:
            return 1;
        default:
            return {};
    }
}

struct Token {
    TokenType type;
    optional<string> value;
};


class Tokeniser {

public:
    inline explicit Tokeniser(const string& src) : m_src(std::move(src)) {
        
    }

    inline vector<Token> tokenise(const string& str) {
        vector<Token> tokens{};

        string buffer;

        while (peek().has_value()) {
            if (isalpha(peek().value())) {
                buffer.push_back(consume());
                while (peek().has_value() && isalnum(peek().value())) {
                    buffer.push_back(consume());
                }

                if (buffer == "exit") {
                    tokens.push_back({TokenType::_exit});
                    buffer.clear();
                    continue;
                } else if (buffer == "var") {
                    tokens.push_back({ TokenType::var});
                    buffer.clear();
                    continue;
                } else {
                    tokens.push_back({.type = TokenType::ident, .value = buffer});
                    buffer.clear();
                    continue;
                }
            } else if (isdigit(peek().value())) {
                buffer.push_back(consume());
                while (peek().has_value() && isdigit(peek().value())) {
                    buffer.push_back(consume());
                }
                tokens.push_back({TokenType::int_lit, buffer});
                buffer.clear();
                continue; 
            } else if (peek().value() == '(') {
                consume();
                tokens.push_back({TokenType::open_paren, nullopt});
                continue;
            } else if (peek().value() == ')') {
                consume();
                tokens.push_back({TokenType::close_paren, nullopt});
                continue;
            } else if (peek().value() == ';') {
                consume();
                tokens.push_back({TokenType::semi, nullopt});
                continue;
            } else if (peek().value() == '=') {
                consume();
                tokens.push_back({TokenType::eq, nullopt});
                continue;
            } else if (peek().value() == '*') {
                consume();
                tokens.push_back({TokenType::star, nullopt});
                continue;
            } else if (peek().value() == '+') {
                consume();
                tokens.push_back({TokenType::plus, nullopt});
                continue;
            } else if (isspace(peek().value())) {
                consume();
                continue;
            } else {
                cout << "Error, invalid character" << endl;
                exit(EXIT_FAILURE);
            }
            
        }
        m_index = 0;
        return tokens;
    }

private:

    [[nodiscard]] inline optional<char> peek(int n=0) const {
        if (m_index + n >= m_src.length()) {
            return {};
        } else {
            return m_src[m_index + n];
        }
    }

    inline char consume() {
        return m_src[m_index++];
    }

    const string& m_src;
    int m_index = 0;


};