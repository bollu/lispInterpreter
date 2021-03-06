#include <assert.h>
#include <iostream> //eugh
#include "interpreter.h"
#include "environment.h"

typedef Int (*arith_op_int)(Int a, Int b);
typedef Real (*arith_op_float)(Real a, Real b);

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


Datum apply_binary_arith_op(std::deque<Datum> params, Interpreter &interpreter, arith_op_int op_int, arith_op_float op_float) {

    assert(params.size() == 2);

    auto datum1 = interpreter.eval(params[0]);
    auto datum2 = interpreter.eval(params[1]);

    if((datum1.flags & Datum::DATUM_TYPE_INT) && (datum2.flags & Datum::DATUM_TYPE_INT)) {
        Int result = op_int(datum1.i, datum2.i);
        return Datum::new_int(result);
    } else {
        Real f1;
        if(datum1.flags & Datum::DATUM_TYPE_INT) {
            f1 = datum1.i;
        } else if (datum1.flags & Datum::DATUM_TYPE_FLOAT) {
            f1 = datum1.f;
        } else {
            assert(false && "first parameter is not a number");
        }

        Real f2;
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

typedef bool (*logical_op_int)(Int a, Int b);
typedef bool (*logical_op_float)(Real a, Real b);
typedef bool (*logical_op_string)(std::string a, std::string b);

Datum apply_binary_logical_op(std::deque<Datum> params, Interpreter &interpreter, logical_op_int op_int, logical_op_float op_float, logical_op_string op_string) {

    //binary operator - has 2 params
    assert(params.size() == 2);

    auto datum1 = params[0];
    auto datum2 = params[1];

    if((datum1.flags & Datum::DATUM_TYPE_INT) && (datum2.flags & Datum::DATUM_TYPE_INT)) {
        bool result = op_int(datum1.i, datum2.i);
        return Datum::new_bool(result);
    }
    //if both of them are either ints of floats, upcast both to float and have fun 
    else if ((datum1.flags & (Datum::DATUM_TYPE_INT | Datum::DATUM_TYPE_FLOAT)) && 
            (datum2.flags & (Datum::DATUM_TYPE_INT | Datum::DATUM_TYPE_FLOAT))){
        Real f1;
        if(datum1.flags & Datum::DATUM_TYPE_INT) {
            f1 = datum1.i;
        } else if (datum1.flags & Datum::DATUM_TYPE_FLOAT) {
            f1 = datum1.f;
        } else {
            assert(false && "first parameter is not a number");
        }

        Real f2;
        if(datum2.flags & Datum::DATUM_TYPE_INT) {
            f2 = datum2.i;
        } else if (datum2.flags & Datum::DATUM_TYPE_FLOAT) {
            f2 = datum2.f;
        } else {
            assert(false && "second parameter is not a number");
        }

        return Datum::new_bool(op_float(f1, f2));
    }

    if((datum1.flags & Datum::DATUM_TYPE_LIST) && (datum2.flags & Datum::DATUM_TYPE_LIST)) {
        assert(false && "unimplemented");
    }

    assert(false && "types don't match in logical operator params");
}

template<typename T>
bool greater_binary_op(T a, T b) {
    return a > b;
}

template<typename T>
bool greater_or_equal_binary_op(T a, T b) {
    return a >= b;
}

template<typename T>
bool equals_binary_op(T a, T b) {
    return a == b;
}

template<typename T>
bool not_equals_binary_op(T a, T b) {
    return a != b;
}

template<typename T>
bool lesser_binary_op(T a, T b) {
    return a < b;
}

template<typename T>
bool lesser_or_equals_binary_op(T a, T b) {
    return a < b;
}


std::deque<Datum> eval_args(std::deque<Datum> to_eval_list, Interpreter &interpreter) {
    std::deque<Datum> evald;

    for(auto datum : to_eval_list) {
        evald.push_back(interpreter.eval(datum));
    }
    return evald;
}

Datum eval_function(Datum function_datum, std::deque<Datum> arguments_list, Interpreter &interpreter) {
    assert(function_datum.flags & Datum::DATUM_TYPE_LAMBDA);
    
    assert(function_datum.function != NULL);
    
    if (arguments_list.size() != function_datum.function->formal_params.size()) {
        std::cout<<"\n>>expected no. of arguments:"<<function_datum.function->formal_params.size()<<" | given: "<<arguments_list.size()<<"\n";
    }
    assert(arguments_list.size() == function_datum.function->formal_params.size());
    
    
    std::shared_ptr<Datum> function_body = function_datum.function->body;
    std::shared_ptr<Environment> fn_envt = function_datum.function->envt;
    
    Environment &old_envt(interpreter.current_envt);
    {
        interpreter.current_envt = *fn_envt;

       
        
        for(std::deque<Datum>::size_type param_index = 0; param_index < arguments_list.size(); param_index++) {
            std::string param_name = function_datum.function->formal_params[param_index].s;
            Datum value = arguments_list[param_index];
            
           interpreter.current_envt.set_symbol(param_name, value); 
        }


        Datum return_value = interpreter.eval(*function_body);

        interpreter.current_envt = old_envt;
    
        return return_value;
    }
    assert(false && "eval unimplemented");
    
}

Datum Interpreter::eval(Datum to_eval) {
    if(to_eval.flags & (Datum::DATUM_TYPE_INT | 
                Datum::DATUM_TYPE_FLOAT | 
                Datum::DATUM_TYPE_STRING |
                Datum::DATUM_TYPE_BOOLEAN)) {

        return to_eval;

    }
    else if(to_eval.flags & Datum::DATUM_TYPE_SYMBOL) {        
        std::string variable_name = to_eval.s;

        if(variable_name == "+") {
            return Datum::new_symbol("+");
        }
        if(variable_name == "*") {
            return Datum::new_symbol("*");
        }
        if(variable_name == "/") {
            return Datum::new_symbol("/");
        }
        if(variable_name == "%") {
            return Datum::new_symbol("%");
        }
        if(variable_name == "let") {
            return Datum::new_symbol("let");
        }
        if(variable_name == "lambda") {
            return Datum::new_symbol("\\");
        }
        if(variable_name == "freeze") {
            return Datum::new_symbol("freeze");
        }

        Datum value = this->current_envt.get_symbol(variable_name);

        return value;
    }
    else if(to_eval.flags & Datum::DATUM_TYPE_LAMBDA) {
        assert(false && "unimplemented lambda execution");
    }
    else if(to_eval.flags & Datum::DATUM_TYPE_LIST) {
        

        Datum function_call_variable = eval(to_eval.list[0]);
            
        //have a lambda, has __no name__ cause it's a lambda! so just execute it
        if(function_call_variable.flags & Datum::DATUM_TYPE_LAMBDA) {

            std::deque<Datum>  params = eval_args(std::deque<Datum>(to_eval.list.begin() + 1, to_eval.list.end()), *this); 

            Datum return_value = eval_function(function_call_variable, 
                    params, *this);
            return return_value;
        }

        
        assert(function_call_variable.flags & Datum::DATUM_TYPE_SYMBOL);

        std::string function_name = function_call_variable.s;
        
        if(function_name == "freeze" || function_name == "~") {
            //only function where we DO NOT eval the args cause the entire
            //point is to *not* eval

            assert(to_eval.list.size() == 2 && "requires 1 operand");
            Datum freeze_list = to_eval.list[1]; //(~ (rest of list))
            assert((freeze_list.flags & Datum::DATUM_TYPE_LIST) && "freeze must recieve list");
            std::vector<Datum> unevaluated = std::vector<Datum>(freeze_list.list.begin(), freeze_list.list.end()); //freeze_list[0] = ~

            return Datum::new_list(unevaluated);
        }

        else if(function_name == "if") {
            assert(to_eval.list.size() == 3 || to_eval.list.size() == 4 && "requires condition, success, <failure case optional>");

            Datum condition = this->eval(to_eval.list[1]);
            assert(condition.flags & Datum::DATUM_TYPE_BOOLEAN && "conditional expected");

            if(condition.b) {
                return this->eval(to_eval.list[2]);

            } else {
                if (to_eval.list.size() == 4) {
                    return this->eval(to_eval.list[3]);
                } else {
                    //assert(false && "conditional was false with no else branch");
                    return Datum::new_bottom();
                }
            }
        }

        else if(function_name == "let") {
            assert(to_eval.list.size() == 3 && "let requires a value binding");
            assert((to_eval.list[1].flags & Datum::DATUM_TYPE_SYMBOL) && "first operand is variable");

            std::string name = to_eval.list[1].s;
            Datum value = this->eval(to_eval.list[2]);

            this->current_envt.set_symbol(name, value);
            return value;
        }
        else if(function_name == "\\" || function_name == "lambda") {
            assert(to_eval.list.size() == 3 && "% requires parameter list and body");

            //TODO - do I need a eval here? I have __no clue__
            Datum formal_params = to_eval.list[1];
            Datum body = to_eval.list[2];
            
            //assert(formal_params.flags & Datum::DATUM_TYPE_LIST); 
            //assert(body.flags & Datum::DATUM_TYPE_LIST); 

            return Datum::new_lambda(formal_params.list, body, this->current_envt);
        }
        

        //NORMAL FORMS---------------
        std::deque<Datum>  params = eval_args(std::deque<Datum>(to_eval.list.begin() + 1, to_eval.list.end()), *this); 

        if(function_name == "+") {
            assert(params.size() == 2&& "+ requires 2 operands");
            return apply_binary_arith_op(params, *this, add_binary_op<Int>, add_binary_op<Real>);       

        } 
        else if(function_name == "-") {
            assert(params.size() == 2 && "- requires 2 operands");
            return apply_binary_arith_op(params, *this, sub_binary_op<Int>, sub_binary_op<Real>);       
        }
        else if(function_name == "*") {
            assert(params.size() == 2 && "* requires 2 operands");
            return apply_binary_arith_op(params, *this, mult_binary_op<Int>, mult_binary_op<Real>);       

        } 
        else if(function_name == "/") {
            assert(params.size() == 2 && "/* requires 2 operands");
            return apply_binary_arith_op(params, *this, div_binary_op<Int>, div_binary_op<Real>);       
        }
        else if(function_name == "%") {
            assert(params.size() == 2 && "% requires 2 operands");
            assert((params[1].flags & Datum::DATUM_TYPE_INT));
            assert((params[2].flags & Datum::DATUM_TYPE_INT));


            return Datum::new_int(params[1].i % params[2].i);
        }
        else if(function_name == "eval" || function_name == "!!") { 
            assert(params.size() == 1 && "requires 1 operand");
            //we call eval on the parameters of it to apply them once so that compound statements
            //are factored
            //Datum list_param = this->eval(params[1]);            

            //now we actually eval them because that's what we're supposed to do
            return this->eval(params[0]);
        }

        else if (function_name == "car") {
            assert(params.size() == 1 && "requires 1 operand");

            Datum car = params[0];
            assert(car.flags & Datum::DATUM_TYPE_LIST);
            assert(car.list.size() > 0);
            return car.list[0];
        }
        else if(function_name == "cdr") {
            assert(params.size() == 1 && "requires 1 operand");

            Datum cdr_datum = params[0];
            assert(cdr_datum.flags & Datum::DATUM_TYPE_LIST);

            std::vector<Datum> cdr(cdr_datum.list.begin() + 1, cdr_datum.list.end());

            return Datum::new_list(cdr);


        }
        else if (function_name == "prepend") {
            assert(params.size() == 2 && "requires 2 operands");
            assert(params[2].flags & Datum::DATUM_TYPE_LIST);

            Datum element = params[1];

            Datum operand_list = params[2];
            operand_list.list.push_front(element);

            return operand_list;
        }
        else if (function_name == "append") {
            assert(params.size() == 2 && "requires 2 operands");
            assert(params[2].flags & Datum::DATUM_TYPE_LIST);

            Datum element = params[1];

            Datum operand_list = params[2];
            operand_list.list.push_back(element);

            return operand_list;
        }
        
        else {

            //custom function

            Datum function_datum = this->current_envt.get_symbol(function_name);
            assert(function_datum.flags & Datum::DATUM_TYPE_LAMBDA);
           
            return eval_function(function_datum, params, *this);
            
            assert(false && "function not found");
        }
    }

}



