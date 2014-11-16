#pragma once
#include "types.h"
#include <string>
#include <deque>

struct Token {
	enum TokenFlags {
		TOKEN_TYPE_OPEN_BRACE = 1<<1,
		TOKEN_TYPE_CLOSE_BRACE = 1<<2,
		TOKEN_TYPE_ATOM = 1<<3,
	};

	Flag flags;
	std::string datum;

	static Token new_open_brace();
	static Token new_close_brace();
	static Token new_atom(std::string datum);

private:
	Token();
	Token(std::string datum);
};

Token token_from_string(const std::string &string);
std::string token_debug_string(const Token &token);
std::deque<Token> tokenize(std::string data);