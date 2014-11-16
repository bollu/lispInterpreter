#pragma once
#include "types.h"
#include <string>
#include <vector>
#include <deque>

struct Token;

struct AST {

        enum ASTFlags {
            AST_NODE_TYPE_NONTERMINAL = 1<<1,
            AST_NODE_TYPE_TERMINAL = 1<<2,
        };

        std::string datum;
        Flag flags;
        std::vector<AST> children;

        void mut_add_child(AST &child);
        static AST new_terminal_node(std::string datum);
        static AST new_nonterminal_node();
private:
	AST();
};

std::string ast_debug_string(const AST &ast);
std::vector<AST> ast_from_tokens(const std::deque<Token> &tokens);