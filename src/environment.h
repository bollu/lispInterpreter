#pragma once
#include "types.h"
#include "datum.h"

#include <string>
#include <map>

struct Environment {
    std::map<std::string, Datum> symbol_table;
    Environment *outer;

public:
    Environment(Environment *outer);
    const Datum &get_symbol(std::string symbol_name) const;
    void set_symbol(std::string name, const Datum &value);

};

std::string environment_debug_string(const Environment &envt);
