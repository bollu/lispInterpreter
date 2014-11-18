#include <stdio.h>
#include <string>
#include <stdint.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <string.h>
#include <deque>
#include <sstream>
#include <map>
#include <stack>


#include "token.h"
#include "datum.h"
#include "ast.h"
#include "environment.h"
#include "interpreter.h"

std::string file_get_contents(const std::string& path) {
    std::ifstream file(path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return content;
}
int main(int argc, char **argv) {
    //force a flush each time IO is done. no more pesky IO stuff >_>
    std::cout.setf( std::ios_base::unitbuf );

    if (argc == 2) {
        assert(argc == 2 && "path to program file needed");
        std::string raw_program = file_get_contents(argv[1]);
        std::cout<<"\n raw:\n"<<raw_program<<"\n"<<std::flush;
        
        auto tokens = tokenize(raw_program);
        Interpreter interpreter;
        
        auto ast_program = ast_from_tokens(tokens);
        
        for(auto ast_expr : ast_program) {
            auto datum = datum_from_ast(ast_expr);
            Datum evald = interpreter.eval(datum); 
            std::cout<<"\n"<<datum_debug_string(evald);
        }

    }
    else {
        Interpreter interpreter;
        while(1) {
            char command[1024 * 1024];
            std::cout<<"\n>>";
            std::cin.getline(command, 1024 * 1024);

            auto tokens = tokenize(std::string(command));
            auto ast_program = ast_from_tokens(tokens);

            for(auto ast_expr : ast_program) {
                auto datum = datum_from_ast(ast_expr);
                Datum evald = interpreter.eval(datum); 
                std::cout<<""<<datum_debug_string(evald);
            }
        }

    }


    return 0;
}

