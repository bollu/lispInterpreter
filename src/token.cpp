#include "token.h"


Token Token::new_open_brace() {
	Token t;
	t.flags = Token::TOKEN_TYPE_OPEN_BRACE;
	return t;
}

Token Token::new_close_brace() {
	Token t;
	t.flags = Token::TOKEN_TYPE_CLOSE_BRACE;
	return t;
}

Token Token::new_atom(std::string datum) {
	Token t(datum);
	t.flags = Token::TOKEN_TYPE_ATOM;
	return t;
}

Token::Token() { this->flags = 0; this->datum = ""; }
Token::Token(std::string datum) : flags(0), datum(datum){};


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
