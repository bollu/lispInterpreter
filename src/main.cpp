#include <stdio.h>
#include <string>
#include <stdint.h>
#include <vector>
#include <sstream>
#include <iostream>


typedef char * lStr;
typedef int lInt;
typedef float lFloat;

typedef int64_t i64;
typedef int32_t i32;
typedef float f32;

enum TokenType {
    TokenOpenBrace,
    TokenCloseBrace,
    TokenAtom,
};


struct Token {
    TokenType type;
    std::string value;
};


Token str_to_token(std::string &string) {
    Token t;

    if(string == "(") {
        t.type = TokenOpenBrace;
        return t;
    }
    else if (string == ")") {
        t.type = TokenCloseBrace;
    }
    else {
        t.type = TokenAtom;
        t.value = string;
    }

    return t;
}

std::string debug_string_token(Token &token) {
    if(token.type == TokenOpenBrace) {
        return "OpenBrace";
    }
    else if(token.type == TokenCloseBrace) {
        return "CloseBrace";
    }
    else if(token.type == TokenAtom) {
        return "Atom: \'" + token.value + "\'";
    }

    return "UNKNOWN TOKEN TYPE";
}

std::vector<Token> tokenize(std::string data) {
    std::vector<Token> tokens;
    std::string current_string;

    bool string_build_ended = false;

    for(auto ch : data) {
        if(ch == ' ' || ch == ' ' || ch == ' ') {
            string_build_ended = true;
        }
        else {
            string_build_ended = false;

            //you've found a valid character, push it back
            current_string.push_back(ch);

            //the string is ONLY an ( or a ), so we gotta break
            if(current_string == "(" || current_string == ")") {
                string_build_ended = true;
            }
        }

        if(string_build_ended && current_string.size() > 0) {
                tokens.push_back(str_to_token(current_string));
                current_string.clear();
        }
    }
    return tokens;
}

int main() {
    auto tokens = tokenize("( bollu ) )");
    for(auto token : tokens) {
        std::cout<<" "<<debug_string_token(token);
    }

    return 0;
}
