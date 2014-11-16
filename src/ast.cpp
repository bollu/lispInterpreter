#include <string>
#include <deque>
#include <assert.h>
#include <vector>

#include "ast.h"
#include "token.h"

AST __ast_from_tokens(std::deque<Token> &tokens);

AST AST::new_terminal_node(std::string datum) {
    AST ast;
    ast.flags = AST_NODE_TYPE_TERMINAL;
    ast.datum = datum;

    return ast;
}

AST AST::new_nonterminal_node() {
    AST ast;
    ast.flags = AST_NODE_TYPE_NONTERMINAL;

    return ast;
}

void AST::mut_add_child(AST &child) {
    this->children.push_back(child);
}

AST::AST() { this->flags = 0; this->datum = ""; };


std::string ast_debug_string(const AST &ast) {
    if(ast.flags & AST::AST_NODE_TYPE_TERMINAL) {
        return ast.datum;
    };

    std::string debug_string = "[ ";

    for(auto child : ast.children) {
        debug_string += "|" + ast_debug_string(child) + "|";
        debug_string += " ";
    }

    debug_string += "]";
    return debug_string;
}



AST __ast_from_tokens(std::deque<Token> &tokens) {
    if(tokens.size() == 0) {
        assert(false && "tokens.size() = 0");
    }

    auto head = tokens[0];
    tokens.pop_front();


    if(head.flags & Token::TOKEN_TYPE_ATOM) {
        AST child = AST::new_terminal_node(head.datum);
        return child;
    }
    else if(head.flags & Token::TOKEN_TYPE_OPEN_BRACE) {
        AST parent = AST::new_nonterminal_node();
        while(1) {

            auto token = tokens[0];
            tokens.pop_front();

            if(token.flags & Token::TOKEN_TYPE_CLOSE_BRACE) {
                return parent;
            } else {
                //rewind since we didn't find a close brace
                tokens.push_front(token);

                auto child = __ast_from_tokens(tokens);
                parent.mut_add_child(child);
            }
        }
    }
    else if(head.flags & Token::TOKEN_TYPE_CLOSE_BRACE) {
        assert(false && "found unmatched bracket");
    }

    assert(false && "--reached end of parseExpr--");
};


std::vector<AST> ast_from_tokens(const std::deque<Token> &tokens) {
    std::deque<Token> tokens_copy(tokens);

    std::vector<AST> expressions;

    while(tokens_copy.size() > 0) {
        expressions.push_back(__ast_from_tokens(tokens_copy));
    }

    return expressions;
}
