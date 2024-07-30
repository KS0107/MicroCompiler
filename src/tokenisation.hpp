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
    star,
    sub,
    div,
    open_brace,
    close_brace,
    if_,
    elif,
    else_
};

inline std::string to_string(const TokenType type) {
    switch (type) {
        case TokenType::_exit:
            return "exit";
        case TokenType::int_lit:
            return "integer literal";
        case TokenType::semi:
            return "';'";
        case TokenType::open_paren:
            return "'('";
        case TokenType::close_paren:
            return "')'";
        case TokenType::ident:
            return "identifier";
        case TokenType::var:
            return "'var'";
        case TokenType::eq:
            return "'='";
        case TokenType::plus:
            return "'+'";
        case TokenType::star:
            return "'*'";
        case TokenType::sub:
            return "'-'";
        case TokenType::div:
            return "'/'";
        case TokenType::open_brace:
            return "'{'";
        case TokenType::close_brace:
            return "'}'";
        case TokenType::if_:
            return "'if'";
        case TokenType::elif:
            return "'elif'";
        case TokenType::else_:
            return "'else'";
    }
}

inline optional<int> bin_prec(TokenType type) {
    switch (type) {
        case TokenType::plus:
        case TokenType::sub:
            return 1;
        case TokenType::star:
        case TokenType::div:
            return 2;
        default:
            return {};
    }
}

struct Token {
    TokenType type;
    int line;
    optional<string> value {};
};


class Tokeniser {

public:
    inline explicit Tokeniser(const string& src) : m_src(std::move(src)) {
        
    }

    inline vector<Token> tokenise(const string& str) {
        vector<Token> tokens{};
        int line_count = 1;
        string buffer;
        while (peek().has_value()) {
            if (isalpha(peek().value())) {
                buffer.push_back(consume());
                while (peek().has_value() && isalnum(peek().value())) {
                    buffer.push_back(consume());
                }

                if (buffer == "exit") {
                    tokens.push_back({TokenType::_exit, line_count});
                    buffer.clear();
                    continue;
                } else if (buffer == "var") {
                    tokens.push_back({ TokenType::var, line_count});
                    buffer.clear();
                    continue;
                } else if (buffer == "if") {
                    tokens.push_back({ TokenType::if_, line_count});
                    buffer.clear();
                    continue;
                } else if (buffer == "elif") {
                    tokens.push_back({ TokenType::elif, line_count});
                    buffer.clear();
                    continue;
                } else if (buffer == "else") {
                    tokens.push_back({ TokenType::else_, line_count});
                    buffer.clear();
                    continue;
                } else {
                    tokens.push_back({TokenType::ident, line_count, buffer});
                    buffer.clear();
                    continue;
                }
            } else if (isdigit(peek().value())) {
                buffer.push_back(consume());
                while (peek().has_value() && isdigit(peek().value())) {
                    buffer.push_back(consume());
                }
                tokens.push_back({TokenType::int_lit, line_count, buffer});
                buffer.clear();
                continue; 
            } else if (peek().value() == '/' && peek(1).has_value() && peek(1).value() == '/') {
                consume();
                consume();
                while (peek().has_value() && peek().value() != '\n' && peek().value() != '\r') {
                    consume(); 
                }
                consume(); 

                if (peek().has_value() && peek().value() == '\n') {
                    consume();
                }
            }  else if (peek().value() == '/' && peek(1).has_value() && peek(1).value() == '*') {
                consume(); 
                consume(); 
                
                while (peek().has_value()) {
                    if (peek().value() == '*' && peek(1).has_value() && peek(1).value() == '/') {
                        consume(); 
                        consume(); 
                        break;
                    }
                    consume();
                }
            }
            else if (peek().value() == '(') {
                consume();
                tokens.push_back({TokenType::open_paren, line_count});
                continue;
            } else if (peek().value() == ')') {
                consume();
                tokens.push_back({TokenType::close_paren, line_count});
                continue;
            } else if (peek().value() == ';') {
                consume();
                tokens.push_back({TokenType::semi, line_count});
                continue;
            } else if (peek().value() == '=') {
                consume();
                tokens.push_back({TokenType::eq, line_count});
                continue;
            } else if (peek().value() == '*') {
                consume();
                tokens.push_back({TokenType::star, line_count});
                continue;
            } else if (peek().value() == '+') {
                consume();
                tokens.push_back({TokenType::plus, line_count});
                continue;
            } else if (peek().value() == '-') {
                consume();
                tokens.push_back({TokenType::sub, line_count});
                continue;
            } else if (peek().value() == '/') {
                consume();
                tokens.push_back({TokenType::div, line_count});
                continue;
            } else if (peek().value() == '{') {
                consume();
                tokens.push_back({TokenType::open_brace, line_count});
                continue;
            } else if (peek().value() == '}') {
                consume();
                tokens.push_back({TokenType::close_brace, line_count});
                continue;
            } else if (peek().value() == '\n') {
                consume();
                line_count++;
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