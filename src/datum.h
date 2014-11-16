#pragma once
#include "types.h"
#include <deque>
#include <string>
#include <vector>
#include <memory>

struct AST;
struct Environment;

struct Datum {
    enum DatumFlags {
        DATUM_TYPE_INT = 1<<1,
        DATUM_TYPE_FLOAT = 1<<2,
        DATUM_TYPE_STRING = 1<<3,
        DATUM_TYPE_LIST = 1<<4,
        DATUM_TYPE_SYMBOL = 1<<5,
        DATUM_TYPE_BOOLEAN = 1<<6,
        DATUM_TYPE_BOTTOM = 1<<7,
        DATUM_TYPE_LAMBDA = 1<<8,
    };
    Flag flags;

    Int i;
    Real f;
    std::string s;
    std::deque<Datum> list;
    bool b;
    
    struct Function {
        std::deque<Datum> formal_params;
        std::shared_ptr<Datum> body;
        std::shared_ptr<Environment> envt;
        
        Function(const std::deque<Datum> &_formal_params, 
                Datum _body, 
                Environment &parent_envt); 
      } *function;


    static Datum new_int(Int i);
    static Datum new_float(Real f);
    static Datum new_string(std::string s);
    static Datum new_symbol(std::string s);
    static Datum new_bool(bool b);
    static Datum new_bottom();

    static Datum new_lambda(const std::deque<Datum> &formal_params, 
                              Datum body,
                              Environment &parent_envt);
    static Datum new_list(const std::vector<AST> &list_members);
    static Datum new_list(const std::vector<Datum> &list_members);


private:
    Datum();
};


Datum datum_from_ast(const AST &ast);
std::string datum_debug_string(const Datum &datum);
