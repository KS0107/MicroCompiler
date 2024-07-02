#pragma once

#include <sstream>
#include "parser.hpp"

class Generator {
    
public:
    inline Generator(NodeExit root) : m_root(std::move(root)) {

    };

   [[nodiscard]] std::string generate() const {
        stringstream out;
        out << ".global _start\n_start:\n";
        out << "   mov x0, #" << m_root.expr.int_lit.value.value() << "\n";
        out << "   mov  x16, #1\n";
        out << "   svc #0";
        return out.str();
    }

private:
    const NodeExit m_root;


};