#pragma once

#include <vector> 
#include "tokenisation.hpp" 

struct NodeExpr {
    Token int_lit;
};

struct NodeExit {
    NodeExpr expr;
};




class Parser {
public:
    inline explicit Parser(vector<Token> tokens) : m_tokens(std::move(tokens)) {
    
        
    }
    optional<NodeExpr> parse_expr() {
        if (peek().has_value() && peek().value().type == TokenType::int_lit) {
            return NodeExpr{.int_lit = consume()};
        } else {
            return {};
        }
    }

    optional<NodeExit> parse() {
        optional<NodeExit> ExitNode;
        while (peek().has_value()) {
            if (peek().value().type == TokenType::_exit) {
                consume();
                if (auto node_expr = parse_expr()) {
                    ExitNode = NodeExit{.expr = node_expr.value() };
                } else {
                    std::cerr << "Invalid expression" << endl;
                    exit(EXIT_FAILURE);
                }

                if (peek().has_value() && peek().value().type == TokenType::semi) {
                    consume();
                } else {
                    std::cerr << "Invalid expression" << endl;
                    exit(EXIT_FAILURE);
                }

            }
        }
        m_index = 0;
        return ExitNode;
    }




private:

    [[nodiscard]] inline optional<Token> peek(int n=1) const {
        if (m_index + n > m_tokens.size()) {
            return {};
        } else {
            return m_tokens[m_index];
        }
    }

    inline Token consume() {
        return m_tokens[m_index++];
    }

    const vector<Token> m_tokens;
    size_t m_index = 0;
};