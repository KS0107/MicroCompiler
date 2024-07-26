#pragma once

#include "parser.hpp"
#include <cassert>
#include <unordered_map>

class Generator {
public:
    inline explicit Generator(NodeProg prog)
        : m_prog(std::move(prog))
    {
    }

    void gen_term(const NodeTerm* term)
    {
        struct TermVisitor {
            Generator* gen;
            void operator()(const NodeTermIntLit* term_int_lit) const
            {
                gen->m_output << "    mov x0, #" << term_int_lit->int_lit.value.value() << "\n";
                gen->push("x0");
            }
            void operator()(const NodeTermIdent* term_ident) const
            {
                if (gen->m_vars.count(term_ident->ident.value.value()) == 0) {
                    std::cerr << "Undeclared identifier: " << term_ident->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                const auto& var = gen->m_vars.at(term_ident->ident.value.value());
                std::stringstream offset;
                offset << "[sp, #" << (gen->m_stack_size - var.stack_loc - 1) * 16 + 8 << "]";
                gen->m_output << "    ldr x0, " << offset.str() << "\n";
                gen->push("x0");
            }
            void operator()(const NodeTermParen* term_paren) const
            {
                gen->gen_expr(term_paren->expr);
            }
        };
        TermVisitor visitor({ .gen = this });
        std::visit(visitor, term->var);
    }

    void gen_bin_expr(const NodeBinExpr* bin_expr)
    {
        struct BinExprVisitor {
            Generator* gen;
            void operator()(const NodeBinExprSub* sub) const
            {
                gen->gen_expr(sub->rhs);
                gen->gen_expr(sub->lhs);
                gen->pop("x0");
                gen->pop("x1");
                gen->m_output << "    sub x0, x0, x1\n";
                gen->push("x0");
            }
            void operator()(const NodeBinExprAdd* add) const
            {
                gen->gen_expr(add->rhs);
                gen->gen_expr(add->lhs);
                gen->pop("x0");
                gen->pop("x1");
                gen->m_output << "    add x0, x0, x1\n";
                gen->push("x0");
            }
            void operator()(const NodeBinExprMulti* multi) const
            {
                gen->gen_expr(multi->rhs);
                gen->gen_expr(multi->lhs);
                gen->pop("x0");
                gen->pop("x1");
                gen->m_output << "    mul x0, x0, x1\n";
                gen->push("x0");
            }
            void operator()(const NodeBinExprDiv* div) const
            {
                gen->gen_expr(div->rhs);
                gen->gen_expr(div->lhs);
                gen->pop("x0");
                gen->pop("x1");
                gen->m_output << "    udiv x0, x0, x1\n";
                gen->push("x0");
            }
        };

        BinExprVisitor visitor { .gen = this };
        std::visit(visitor, bin_expr->var);
    }

    void gen_expr(const NodeExpr* expr)
    {
        struct ExprVisitor {
            Generator* gen;
            void operator()(const NodeTerm* term) const
            {
                gen->gen_term(term);
            }
            void operator()(const NodeBinExpr* bin_expr) const
            {
                gen->gen_bin_expr(bin_expr);
            }
        };

        ExprVisitor visitor { .gen = this };
        std::visit(visitor, expr->var);
    }

    void gen_stmt(const NodeStmt* stmt)
    {
        struct StmtVisitor {
            Generator* gen;
            void operator()(const NodeStmtExit* stmt_exit) const
            {
                gen->gen_expr(stmt_exit->expr);
                gen->m_output << "    mov x16, #1\n";
                gen->pop("x0");
                gen->m_output << "    svc #0x80\n";
            }
            void operator()(const NodeStmtVar* stmt_let) const
            {
                if (gen->m_vars.find(stmt_let->ident.value.value()) != gen->m_vars.end()) {
                    std::cerr << "Identifier already used: " << stmt_let->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                gen->m_vars.insert({ stmt_let->ident.value.value(), Var { .stack_loc = gen->m_stack_size } });
                gen->gen_expr(stmt_let->expr);
                gen->push("x0");
            }
        };

        StmtVisitor visitor { .gen = this };
        std::visit(visitor, stmt->var);
    }

    [[nodiscard]] std::string gen_prog()
    {
        m_output << ".global _start\n_start:\n";

        for (const NodeStmt* stmt : m_prog.stmts) {
            gen_stmt(stmt);
        }

        m_output << "    mov x0, #0\n";
        m_output << "    mov x16, #1\n";
        m_output << "    svc #0x80\n";
        return m_output.str();
    }

private:
    void push(const std::string& reg)
    {
        m_output << "    sub sp, sp, #16\n";
        m_output << "    str " << reg << ", [sp, #8]\n";
        m_stack_size++;
    }

    void pop(const std::string& reg)
    {
        m_output << "    ldr " << reg << ", [sp, #8]\n";
        m_output << "    add sp, sp, #16\n";
        m_stack_size--;
    }

    struct Var {
        size_t stack_loc;
    };

    const NodeProg m_prog;
    std::stringstream m_output;
    size_t m_stack_size = 0;
    std::unordered_map<std::string, Var> m_vars {};
};
