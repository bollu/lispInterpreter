#pragma once
#include "environment.h"
#include "datum.h"

struct Interpreter {
    Environment global_envt;
    Environment &current_envt;

    Interpreter() : global_envt(NULL), current_envt(global_envt){}
    

    Datum eval(Datum to_eval);
};
