#include <iostream>
#include <assert.h>
#include "environment.h"
#include "datum.h"

Environment::Environment(Environment *outer) {
    this->outer = outer;
}

const Datum &Environment::get_symbol(std::string symbol_name) const {
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

void Environment::set_symbol(std::string name, const Datum &value) {
    symbol_table.insert(std::pair<std::string, Datum>(name, value));  
}

#include <sstream>
std::string environment_debug_string(const Environment &envt) {
    std::stringstream ss;
    ss<<"{\n";
    for(auto child : envt.symbol_table) {
        ss<<"\t"<<child.first<<" => "<<datum_debug_string(child.second)<<"\n";  
    }

    if(envt.outer != NULL) {
            ss<<"\nPARENT:";
            ss<<environment_debug_string(envt.outer);
    }
    ss<<"}\n";

    return ss.str();
};
