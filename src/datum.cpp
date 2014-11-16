#include <assert.h>
#include "environment.h"
#include "datum.h"

#include "ast.h"

Datum::Datum() {
    this->i = -42;
    this->f = -42;
    this->s = "INVALID VALUE";
    this->b = false;
    this->flags = 0;
    this->function = NULL;
};




Datum Datum::new_int(Int i) { 
    Datum v;
    v.flags = DATUM_TYPE_INT;
    v.i = i;
    return v;
}

Datum Datum::new_float(Real f) { 
    Datum v;
    v.flags = DATUM_TYPE_FLOAT;
    v.f = f; 
    return v;
}

Datum Datum::new_string(std::string s) { 
    Datum v;
    v.flags = DATUM_TYPE_STRING;
    v.s = s;
    return v;
}

Datum Datum::new_symbol(std::string s) { 
    Datum v;
    v.flags = DATUM_TYPE_SYMBOL;
    v.s = s;
    return v;
}

Datum Datum::new_bool(bool b) { 
    Datum v;
    v.flags = DATUM_TYPE_BOOLEAN;
    v.b = b;
    return v;
}

Datum Datum::new_bottom() {
    Datum v;
    v.flags = DATUM_TYPE_BOTTOM;
    return v;
}

Datum Datum::new_lambda(const std::deque<Datum> &formal_params, 
        Datum body,
        Environment &parent_envt){
    Datum v;
    v.flags = /*DATUM_TYPE_SYMBOL |*/ DATUM_TYPE_LAMBDA;
    v.function = new Function(formal_params, body, parent_envt);
    return v;
}


Datum Datum::new_list(const std::vector<AST> &list_members) {

    Datum v;
    v.flags = DATUM_TYPE_LIST;
    for(auto member: list_members) {
        v.list.push_back(datum_from_ast(member));
    };
    return v;
}
Datum Datum::new_list(const std::vector<Datum> &list_members) {

    Datum v;
    v.flags = DATUM_TYPE_LIST;
    for(auto member: list_members) {
        v.list.push_back(member);
    };
    return v;
}

#pragma once
#include <iostream>

Datum::Function::Function( const std::deque<Datum> &_formal_params, 
        Datum _body, 
        Environment &parent_envt) : 
    formal_params(_formal_params) {

        this->body = std::shared_ptr<Datum>(new Datum(_body));
        this->envt = std::shared_ptr<Environment>(new Environment(parent_envt));
    }; 

#include <sstream>
#include <iostream> //for no skip whitespace
Datum datum_from_ast(const AST &ast) {

    std::string datum = ast.datum;
    if(ast.flags & AST::AST_NODE_TYPE_TERMINAL) {

        std::stringstream ss;
        ss.str(datum);
        int i; ss>>std::noskipws>>i;

        if(!ss.fail() && ss.eof()) {
            return Datum::new_int(static_cast<int>(i));
        }

        //float
        ss.clear();
        ss.str(datum);
        float f; ss>>f;

        if(!ss.fail() && ss.eof()) {
            return Datum::new_float(static_cast<float>(f));           
        }

        //string
        if(datum.front() == '\"' && datum.back()  == '\"') {
            return Datum::new_string(std::string(datum.begin() + 1, datum.end() - 1));
        }

        //boolean
        if(datum == "true") {
            return Datum::new_bool(true);
        }
        if(datum == "false") {
            return Datum::new_bool(false);
        }
        //variable
        return Datum::new_symbol(datum);    



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

   if(datum.flags & Datum::DATUM_TYPE_BOOLEAN) {
        std::stringstream ss;
        ss<<"datum bool: ";
        if(datum.b) {
            ss<<"true";
        } else {
            ss<<"false";
        }

        return ss.str();
    }
    else if (datum.flags & Datum::DATUM_TYPE_LAMBDA) {
        std::stringstream ss;
        ss<<"datum lambda: Params: ";

        assert(datum.function != NULL);
        ss<<"[";
        for(auto child : datum.function->formal_params) {
            ss<<" "<<datum_debug_string(child); 
        }
        ss<<" ]";


        ss<<" . Body: [ ";
        ss<<datum_debug_string(*datum.function->body);
        ss<<" ]\n";
        return ss.str();


    }
    if(datum.flags & Datum::DATUM_TYPE_LIST) {
        std::stringstream ss;
        ss<<"datum list: {";

        for(auto child : datum.list) {
            ss<<" "<<datum_debug_string(child); 
        }
        ss<<" }";

        return ss.str();

    }
    if(datum.flags & Datum::DATUM_TYPE_SYMBOL) {
        std::stringstream ss;
        ss<<"datum symbol: |"<<datum.s<<"|";

        return ss.str();

    }
 
    else if (datum.flags & Datum::DATUM_TYPE_STRING) {
        std::stringstream ss;
        ss<<"datum string: |";
        ss<<datum.s<<"| ";

        return ss.str();
    }
    else if (datum.flags & Datum::DATUM_TYPE_BOTTOM) {
        std::stringstream ss;
        ss<<"datum bottom";

        return ss.str();
    }
    assert(false && "unknown datum type");
}
