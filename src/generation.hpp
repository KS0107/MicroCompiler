#pragma once

#include "parser.hpp"
#include <cassert>
#include <map>

class Generator {
public:
    inline explicit Generator(NodeProg prog)
        : m_prog(std::move(prog))
    {
    }

    void gen_term(const NodeTerm* term)
    {
        struct TermVisitor {
            Generator& gen;
            void operator()(const NodeTermIntLit* term_int_lit) const
            {
                gen.m_output << "    mov x0, #" << term_int_lit->int_lit.value.value() << "\n";
                gen.push("x0");
            }
            void operator()(const NodeTermIdent* term_ident) const
            {
                auto it = std::find_if(gen.m_vars.cbegin(), 
                    gen.m_vars.cend(),
                    [&](const Var& var) { return var.name == term_ident->ident.value.value();});
                if (it == gen.m_vars.cend()) {
                    std::cerr << "Undeclared identifier: " << term_ident->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                std::stringstream offset;
                offset << "[sp, #" << (gen.m_stack_size - (*it).stack_loc - 1) * 16 + 8 << "]";
                gen.m_output << "    ldr x0, " << offset.str() << "\n";
                gen.push("x0");
            }
            void operator()(const NodeTermParen* term_paren) const
            {
                gen.gen_expr(term_paren->expr);
            }
        };
        TermVisitor visitor({ .gen = *this });
        std::visit(visitor, term->var);
    }

    void gen_bin_expr(const NodeBinExpr* bin_expr)
    {
        struct BinExprVisitor {
            Generator& gen;
            void operator()(const NodeBinExprSub* sub) const
            {
                gen.gen_expr(sub->rhs);
                gen.gen_expr(sub->lhs);
                gen.pop("x0");
                gen.pop("x1");
                gen.m_output << "    sub x0, x0, x1\n";
                gen.push("x0");
            }
            void operator()(const NodeBinExprAdd* add) const
            {
                gen.gen_expr(add->rhs);
                gen.gen_expr(add->lhs);
                gen.pop("x0");
                gen.pop("x1");
                gen.m_output << "    add x0, x0, x1\n";
                gen.push("x0");
            }
            void operator()(const NodeBinExprMulti* multi) const
            {
                gen.gen_expr(multi->rhs);
                gen.gen_expr(multi->lhs);
                gen.pop("x0");
                gen.pop("x1");
                gen.m_output << "    mul x0, x0, x1\n";
                gen.push("x0");
            }
            void operator()(const NodeBinExprDiv* div) const
            {
                gen.gen_expr(div->rhs);
                gen.gen_expr(div->lhs);
                gen.pop("x0");
                gen.pop("x1");
                gen.m_output << "    udiv x0, x0, x1\n";
                gen.push("x0");
            }
        };

        BinExprVisitor visitor { .gen = *this };
        std::visit(visitor, bin_expr->var);
    }

    void gen_scope(const NodeScope* scope) {
        begin_scope();
        for (const NodeStmt* stmt : scope->stmts) {
            gen_stmt(stmt);
        }
        end_scope(); 
    }

    void gen_if_pred(const NodeIfPred* pred, const std::string& end_label) {
        struct PredVisitor {
            Generator& gen;
            const std::string& end_label;

            void operator()(const NodeIfPredElif* elif) const {
                gen.gen_expr(elif->expr);
                gen.pop("x0");
                const std::string label = gen.create_label();
                gen.m_output << "    cmp x0, #0\n";
                gen.m_output << "    beq " << label << "\n";
                gen.gen_scope(elif->scope);
                gen.m_output << "    b " << end_label << "\n";
                if (elif->pred.has_value()) {
                    gen.m_output << label << ":\n";
                    gen.gen_if_pred(elif->pred.value(), end_label);
                }
            }

            void operator()(const NodeIfPredElse* else_) const {
                gen.gen_scope(else_->scope);
            }
        };
        PredVisitor visitor{ .gen = *this, .end_label = end_label };
        std::visit(visitor, pred->var);
    }



    void gen_expr(const NodeExpr* expr)
    {
        struct ExprVisitor {
            Generator& gen;
            void operator()(const NodeTerm* term) const
            {
                gen.gen_term(term);
            }
            void operator()(const NodeBinExpr* bin_expr) const
            {
                gen.gen_bin_expr(bin_expr);
            }
        };

        ExprVisitor visitor { .gen = *this };
        std::visit(visitor, expr->var);
    }

void gen_stmt(const NodeStmt* stmt) {
    struct StmtVisitor {
        Generator& gen;

        void operator()(const NodeStmtExit* stmt_exit) const {
            gen.gen_expr(stmt_exit->expr);
            gen.m_output << "    mov x16, #1\n";
            gen.pop("x0");
            gen.m_output << "    svc #0x80\n";
        }

        void operator()(const NodeStmtVar* stmt_let) const {
            auto it = std::find_if(gen.m_vars.cbegin(), gen.m_vars.cend(),
                [&](const Var& var) { return var.name == stmt_let->ident.value.value(); });
            if (it != gen.m_vars.cend()) {
                std::cerr << "Identifier already used: " << stmt_let->ident.value.value() << std::endl;
                exit(EXIT_FAILURE);
            }
            gen.m_vars.push_back({ .name = stmt_let->ident.value.value(), .stack_loc = gen.m_stack_size });
            gen.gen_expr(stmt_let->expr);
            gen.push("x0");
        }

        void operator() (const NodeStmtAssign* stmt_assign) const {
            auto it = std::find_if(gen.m_vars.cbegin(), gen.m_vars.cend(),
                [&](const Var& var) { return var.name == stmt_assign->ident.value.value(); });
            if (it == gen.m_vars.cend()) {
                std::cerr << "Identifier has not been declared." << stmt_assign->ident.value.value() << std::endl;
                exit(EXIT_FAILURE);
            }
            gen.gen_expr(stmt_assign->expr);
            gen.pop("x0"); // Pop value into x0
            std::stringstream offset;
            offset << "[sp, #" << (gen.m_stack_size - it->stack_loc - 1) * 16 + 8 << "]"; // Calculate memory address
            gen.m_output << "    str x0, " << offset.str() << "\n"; // Store x0 into memory

        }

        void operator()(const NodeScope* scope) const {
            gen.gen_scope(scope);
        }

        void operator()(const NodeStmtIf* stmt_if) const {
            gen.gen_expr(stmt_if->expr);
            gen.pop("x0");
            const std::string label_if_false = gen.create_label();
            const std::string label_end_if = gen.create_label();
            gen.m_output << "    cmp x0, #0\n";
            gen.m_output << "    beq " << label_if_false << "\n";
            gen.gen_scope(stmt_if->scope);
            gen.m_output << "    b " << label_end_if << "\n";
            gen.m_output << label_if_false << ":\n";

            if (stmt_if->pred.has_value()) {
                gen.gen_if_pred(stmt_if->pred.value(), label_end_if);
            }

            gen.m_output << label_end_if << ":\n";
        }
    };
    StmtVisitor visitor{ .gen = *this };
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

    void begin_scope() {
        m_scopes.push_back(m_vars.size());

    }

    void end_scope() {
        size_t pop_count = m_vars.size() - m_scopes.back();
        m_output << "    add sp, sp, #" << pop_count * 16 << "\n";
        m_stack_size -= pop_count;
        for (int i = 0; i < pop_count; i++) {
            m_vars.pop_back();
        }
        m_scopes.pop_back();
    }
    
    std::string create_label() {
        std::string ret = "label" + std::to_string(m_label_count++);
        return ret;
    }

    struct Var {
        std::string name;
        size_t stack_loc;
    };

    const NodeProg m_prog;
    int m_label_count = 0 ;
    std::stringstream m_output;
    size_t m_stack_size = 0;
    std::vector<Var> m_vars {};
    std::vector<size_t> m_scopes {};
};
