#pragma once

#include <sstream>
#include "parser.hpp"
#include <cassert>

class Generator {
    
public:
    inline Generator(NodeProg* prog) : m_prog(std::move(prog)) {

    };
 
    void gen_expr(const NodeExpr* expr) {
        struct ExprVisitor {
            Generator* gen;

            void operator() (const NodeExprIntLit* expr_int_lit) {
                gen->m_output << "    mov x0, #" << expr_int_lit->int_lit.value.value() << "\n";
                gen->push("x0");
            }

            void operator() (const NodeExprIdent* expr_ident) {
                if (gen->m_vars.count(expr_ident->ident.value.value()) == 0) {
                    std::cerr << "Undeclared identifier: " << expr_ident->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
            const auto& var = gen->m_vars.at(expr_ident->ident.value.value());
            std::stringstream offset;
            offset << "[sp, #" << (gen->m_stack_size - var.stack_loc - 1) * 16 + 8 << "]";
            gen->m_output << "    ldr x0, " << offset.str() << "\n";
            gen->push("x0");
            }
            void operator()(const NodeBinExpr* bin_expr) const {
                assert(false);
            }
        };

        ExprVisitor visitor{.gen = this};
        std::visit(visitor, expr->var);
    }

    void gen_stmt(const NodeStmt* stmt)  {
        struct Stmtvisitor {
            Generator* gen;
            void operator() ( const NodeStmtExit* stmt_exit) const{

                gen->gen_expr(stmt_exit->expr);
                gen->m_output << "    mov x16, #1\n";
                gen->pop("x0");
                gen->m_output << "    svc #0x80\n";
    
            }

            void operator() (const NodeStmtVar* stmt_var) const {
                if (gen->m_vars.count(stmt_var->ident.value.value()) > 0) {
                    std::cerr << "Identifier is already declared: " << stmt_var->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                 
                gen->m_vars.insert({stmt_var->ident.value.value(), Var {.stack_loc = gen->m_stack_size}});
                
                gen->gen_expr(stmt_var->expr);
                
            }
        };

        Stmtvisitor visitor { .gen = this};
        std::visit(visitor, stmt->var);
    }


    [[nodiscard]] std::string gen_prog() {
   
        m_output << ".global _start\n_start:\n";

        for (const NodeStmt* stmt : m_prog->stmts) {
            gen_stmt(stmt);
        }
        m_output << "    mov x0, #0\n";
        m_output << "    mov x16, #1\n";
        m_output << "    svc #0x80\n";

        return m_output.str();
    }

private:
    void push(const string& reg) {
        m_output << "    sub sp, sp, #16\n";  
        m_output << "    str " <<  reg << ", [sp, #8]\n";
        m_stack_size++;
    }

    void pop(const string& reg) {
        m_output << "    ldr " << reg << ", [sp, #8]\n";
        m_output << "    add sp, sp, #16\n";
        m_stack_size--;
    }

    struct Var {
        size_t stack_loc;
    };

    const NodeProg* m_prog;
    std::stringstream m_output;
    size_t m_stack_size = 0;
    std::unordered_map<std::string, Var> m_vars {};
};