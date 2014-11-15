#pragma once
#include <stdio.h>
#include <string>
#include <stdint.h>
#include <vector>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <deque>
#include <sstream>
#include <map>
#include <stack>

typedef char * lStr;
typedef int lInt;
typedef float lFloat;
typedef unsigned int Flag;

struct Token {
    enum TokenFlags {
        TOKEN_TYPE_OPEN_BRACE = 1<<1,
        TOKEN_TYPE_CLOSE_BRACE = 1<<2,
        TOKEN_TYPE_ATOM = 1<<3,
    };

    Flag flags;
    std::string datum;

    static Token new_open_brace() {
        Token t;
        t.flags = Token::TOKEN_TYPE_OPEN_BRACE;
        return t;
    }

    static Token new_close_brace() {
        Token t;
        t.flags = Token::TOKEN_TYPE_CLOSE_BRACE;
        return t;
    }

    static Token new_atom(std::string datum) {
        Token t(datum);
        t.flags = Token::TOKEN_TYPE_ATOM;
        return t;
    }

    private:
    Token() { this->flags = 0; this->datum = ""; }
    Token(std::string datum) : datum(datum), flags(0){};
};

Token token_from_string(const std::string &string) {

    if(string == "(") {
        return Token::new_open_brace();
    }
    else if (string == ")") {
        return Token::new_close_brace();
    }
    else {
        return Token::new_atom(string);
    }
}

std::string token_debug_string(const Token &token) {
    if(token.flags & Token::TOKEN_TYPE_OPEN_BRACE) {
        return "OpenBrace";
    }
    else if(token.flags & Token::TOKEN_TYPE_CLOSE_BRACE) {
        return "CloseBrace";
    }
    else if(token.flags & Token::TOKEN_TYPE_ATOM) {
        return "Atom: " + token.datum + "";
    }

    return "UNKNOWN TOKEN TYPE";
}

std::deque<Token> tokenize(std::string data) {
    std::deque<Token> tokens;
    std::string current_string;

    bool string_build_ended = false;
    bool parsing_string = false;

    for(auto it = data.begin(); it != data.end(); ++it) {
        char ch = *it;

        if((ch == ' ' || ch == '\t' || ch == '\n') && !parsing_string) {
            string_build_ended = true;
        }
        else if(ch == '\"') {
            if(parsing_string) { 
                parsing_string = false; 
                string_build_ended = true; 
                current_string.push_back(ch); 
            }
            else { 
                if(current_string.size() > 0) {
                    string_build_ended = true;
                    parsing_string = false;
                    --it;
                } else { 
                    string_build_ended = false;
                    parsing_string = true;
                    current_string.push_back(ch);
                }
            }
        }

        //the string is ONLY an ( or a ), so we gotta break
        else if(ch == ')' || ch == '(') {
            //we already have a string to read
            if(current_string.size()> 0) {
                --it; //rewind the iterator and end this word
                string_build_ended = true;
            }else {
                //this is a fresh string
                current_string.push_back(ch);
                string_build_ended = true;
            }
        }
        //you've found a valid character, push it back
        else {
            string_build_ended = false;
            current_string.push_back(ch);
        }

        if(string_build_ended && current_string.size() > 0) {
            tokens.push_back(token_from_string(current_string));
            current_string.clear();
        }
    }

    //there's some string that's untokenized
    if(current_string.size() > 0) {
        tokens.push_back(token_from_string(current_string));
        current_string.clear();
    }
    return tokens;
}

/*
   class TokenStream {
   std::vector<Token> tokens;
   int position;

   public:
   TokenStream(std::vector<Token> tokens) : tokens(tokens), position(0) {};

   void pop() {
   position++;
   assert((position < this->tokens.size()) && "error: ran out of tokens");
   }
   Token peek() { return tokens[position]; }

   void rewind() { position--; }

   };
   */

struct AST {
    private:
        AST() { this->flags = 0; this->datum = ""; };

    public:

        enum ASTFlags {
            AST_NODE_TYPE_NONTERMINAL = 1<<1,
            AST_NODE_TYPE_TERMINAL = 1<<2,
        };

        std::string datum;
        Flag flags;
        std::vector<AST> children;

        void mut_add_child(AST &child) {
            this->children.push_back(child);
        }

        static AST new_terminal_node(std::string datum) {
            AST ast;
            ast.flags = AST_NODE_TYPE_TERMINAL;
            ast.datum = datum;

            return ast;
        }

        static AST new_nonterminal_node() {
            AST ast;
            ast.flags = AST_NODE_TYPE_NONTERMINAL;

            return ast;
        }
};

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


AST ast_from_tokens(std::deque<Token> tokens) {
    std::deque<Token> tokens_copy(tokens);
    return __ast_from_tokens(tokens_copy);
}

struct Datum;
Datum datum_from_ast(const AST &ast);

struct Datum {
    enum DatumFlags {
        DATUM_TYPE_INT = 1<<1,
        DATUM_TYPE_FLOAT = 1<<2,
        DATUM_TYPE_STRING = 1<<3,
        DATUM_TYPE_LIST = 1<<4,
        DATUM_TYPE_VARIABLE = 1<<5,
    };

    long long i;
    long double f;
    std::string s;
    std::vector<Datum> list;

    Flag flags;

    private:
    Datum() {
        this->i = -42;
        this->f = -42;
        this->s = "INVALID VALUE";
    };


    public:

    static Datum new_int(int i) { 
        Datum v;
        v.flags = DATUM_TYPE_INT;
        v.i = i;
        return v;
    }

    static Datum new_float(float f) { 
        Datum v;
        v.flags = DATUM_TYPE_FLOAT;
        v.f = f; 
        return v;
    }

    static Datum new_string(std::string s) { 
        Datum v;
        v.flags = DATUM_TYPE_STRING;
        v.s = s;
        return v;
    }

    static Datum new_variable(std::string s) { 
        Datum v;
        v.flags = DATUM_TYPE_VARIABLE;
        v.s = s;
        return v;
    }

    static Datum new_list(std::vector<AST> list_members) {

        Datum v;
        v.flags = DATUM_TYPE_LIST;
        for(auto member: list_members) {
            v.list.push_back(datum_from_ast(member));
        };
        return v;
    }
    static Datum new_list(std::vector<Datum> list_members) {

        Datum v;
        v.flags = DATUM_TYPE_LIST;
        for(auto member: list_members) {
            v.list.push_back(member);
        };
        return v;
    }

};

typedef int (*arith_op_int)(int a, int b);
typedef float (*arith_op_float)(float a, float b);

struct Interpreter; //forward decl
Datum eval(Datum to_eval, Interpreter &interpreter); //forward decl
Datum apply_binary_op(Datum eval_list, Interpreter &interpreter, arith_op_int op_int, arith_op_float op_float) {

    //binary operator - has 3 things - the operator and two parameters
    assert(eval_list.flags & Datum::DATUM_TYPE_LIST);
    assert(eval_list.list.size() == 3);

    auto datum1 = eval(eval_list.list[1], interpreter);
    auto datum2 = eval(eval_list.list[2], interpreter);

    if((datum1.flags & Datum::DATUM_TYPE_INT) && (datum2.flags & Datum::DATUM_TYPE_INT)) {
        std::cout<<"\n\tval1 i: "<<datum1.i<<" | val2 i: "<<datum2.i;

        int result = op_int(datum1.i, datum2.i);
        std::cout<<"\n\tresult: "<<result;
        return Datum::new_int(result);
    } else {
        float f1;
        if(datum1.flags & Datum::DATUM_TYPE_INT) {
            f1 = datum1.i;
        } else if (datum1.flags & Datum::DATUM_TYPE_FLOAT) {
            f1 = datum1.f;
        } else {
            assert(false && "first parameter is not a number");
        }

        float f2;
        if(datum2.flags & Datum::DATUM_TYPE_INT) {
            f2 = datum2.i;
        } else if (datum2.flags & Datum::DATUM_TYPE_FLOAT) {
            f2 = datum2.f;
        } else {
            assert(false && "second parameter is not a number");
        }

        return Datum::new_float(op_float(f1, f2));
    }   
}

template<typename T>
T add_binary_op(T var1, T var2) {
    return var1 + var2;
}

template<typename T>
T sub_binary_op(T var1, T var2) {
    return var1 - var2;
}

template<typename T>
T mult_binary_op(T var1, T var2) {
    return var1 * var2;
}


template<typename T>
T div_binary_op(T var1, T var2) {
    return var1 / var2;
}


template<typename T>
T mod_binary_op(T var1, T var2) {
    return var1 % var2;
}






Datum datum_from_ast(const AST &ast) {

    std::string datum = ast.datum;
    if(ast.flags & AST::AST_NODE_TYPE_TERMINAL) {

        std::stringstream ss;
        ss.str(datum);
        int i; ss>>std::noskipws>>i;

        if(!ss.fail() && ss.eof()) {
            return Datum::new_int((int)i);
        }

        //float
        ss.clear();
        ss.str(datum);
        float f; ss>>f;

        if(!ss.fail() && ss.eof()) {
            return Datum::new_float((float)f);           
        }

        //string
        if(datum.front() == '\"' && datum.back()  == '\"') {
            return Datum::new_string(std::string(datum.begin() + 1, datum.end() - 1));
        }

        //variable
        return Datum::new_variable(datum);    



    }
    else {
        if(ast.flags & AST::AST_NODE_TYPE_NONTERMINAL) {
            return Datum::new_list(ast.children);
        }
        assert(false && "unimplemented");
    }
}

std::string datum_debug_string(const Datum &datum) {
    if(datum.flags & Datum::DATUM_TYPE_INT) {
        std::stringstream ss;
        ss<<"datum int: "<<datum.i;

        return ss.str();
    }
    if(datum.flags & Datum::DATUM_TYPE_FLOAT) {
        std::stringstream ss;
        ss<<"datum float: "<<datum.f;

        return ss.str();
    }

    if(datum.flags & Datum::DATUM_TYPE_VARIABLE) {
        std::stringstream ss;
        ss<<"datum variable: |"<<datum.s<<"|";

        return ss.str();

    }


    if(datum.flags & Datum::DATUM_TYPE_LIST) {
        std::stringstream ss;
        ss<<"datum list: {";

        for(auto child : datum.list) {
            ss<<" "<<datum_debug_string(child); 
        }
        ss<<"}";

        return ss.str();

    }
    else if (datum.flags & Datum::DATUM_TYPE_STRING) {
        std::stringstream ss;
        ss<<"datum string:|";
        ss<<datum.s<<"| ";

        return ss.str();
    }

    assert(false && "unknown datum type");
}


struct Interpreter;
struct Environment {
    std::map<std::string, Datum> symbol_table;
    Environment *outer;

    public:
    Environment(Environment *outer) {
        this->outer = outer;
    }

    Datum get_symbol(std::string symbol_name) {
        auto datum_it = this->symbol_table.find(symbol_name);

        if(datum_it == this->symbol_table.end()) {
            if(this->outer == NULL) {
                std::cout<<"unable to find symbol: "<<symbol_name;
                assert(false && "symbol not found");
            }
            else {
                return this->outer->get_symbol(symbol_name);
            }
        }
        else {
            return datum_it->second;
        }

    }
};

struct Interpreter {
    Environment global_envt;
    Environment *current_envt;

    Interpreter() : global_envt(NULL) {
        this->current_envt = NULL;
    }
};

Datum eval(Datum to_eval, Interpreter &interpreter) {
    if(to_eval.flags & Datum::DATUM_TYPE_INT) {
        return to_eval;
    }

    if(to_eval.flags & Datum::DATUM_TYPE_FLOAT) {
        return to_eval;
    }

    if(to_eval.flags & Datum::DATUM_TYPE_STRING) {
        return to_eval;
    }

    if(to_eval.flags & Datum::DATUM_TYPE_LIST) {
        //normal forms and special forms go here

        //index 0 must be a function name
        assert(to_eval.list[0].flags & Datum::DATUM_TYPE_VARIABLE);
        std::string function_name = to_eval.list[0].s;


        if(function_name == "+") {
            assert(to_eval.list.size() == 3 && "+ requires 2 operands");
            return apply_binary_op(to_eval, interpreter, add_binary_op<int>, add_binary_op<float>);       

        } 
        else if(function_name == "-") {
            assert(to_eval.list.size() == 3 && "- requires 2 operands");
            return apply_binary_op(to_eval, interpreter, sub_binary_op<int>, sub_binary_op<float>);       
        }
        if(function_name == "*") {
            assert(to_eval.list.size() == 3 && "+ requires 2 operands");
            return apply_binary_op(to_eval, interpreter, mult_binary_op<int>, mult_binary_op<float>);       

        } 
        else if(function_name == "/") {
            assert(to_eval.list.size() == 3 && "- requires 2 operands");
            return apply_binary_op(to_eval, interpreter, div_binary_op<int>, div_binary_op<float>);       
        }
        else if(function_name == "%") {
            assert(to_eval.list.size() == 3 && "- requires 2 operands");
            assert((to_eval.list[1].flags & Datum::DATUM_TYPE_INT));
            assert((to_eval.list[2].flags & Datum::DATUM_TYPE_INT));


            return Datum::new_int(to_eval.list[1].i % to_eval.list[2].i);
        }
        
        else if(function_name == "freeze") {
            assert(to_eval.list.size() == 2 && "requires 1 operand");
            assert(to_eval.list[1].flags & Datum::DATUM_TYPE_LIST);

            return to_eval.list[1];
        }
    
        else if (function_name == "car") {
            assert(to_eval.list.size() == 2 && "requires 1 operand");
            assert(to_eval.list[1].flags & Datum::DATUM_TYPE_LIST);

       
            Datum list_param = to_eval.list[1];
            Datum car = list_param.list[0];
            return car;
        }
        else if(function_name == "cdr") {
             assert(to_eval.list.size() == 2 && "requires 1 operand");
            assert(to_eval.list[1].flags & Datum::DATUM_TYPE_LIST);

            Datum list_param = to_eval.list[1];
            std::vector<Datum> cdr(list_param.list.begin() + 1, list_param.list.end());
            
            return Datum::new_list(cdr);

        }
        else {
            assert(false && "function not found");
        }

    }
}

int main() {
    //force a flush each time IO is done. no more pesky IO stuff >_>
    std::cout.setf( std::ios_base::unitbuf );
    
    auto tokens = tokenize("(cdr (1 23 \"bollu\"))");
    std::cout<<"\nToken: "<<token_debug_string(tokens[1]);
    std::cout<<"\nAST:\n-----\n"<<ast_debug_string(ast_from_tokens(tokens))<<"\n";
    std::cout<<"\n\nDatum:\n-----\n"<<datum_debug_string(datum_from_ast(ast_from_tokens(tokens)));

    Interpreter interpreter;
    Datum evald = eval(datum_from_ast(ast_from_tokens(tokens)), interpreter); 
    std::cout<<"\n\nEVAL:\n-----\n"<<datum_debug_string(evald);

    return 0;
}

