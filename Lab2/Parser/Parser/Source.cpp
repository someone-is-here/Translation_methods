#include <iostream>
#include <vector>
#include <string>
#include "Lexer.cpp"

using namespace std;

int main() {
	Lexer lex = Lexer("script.txt");
	lex.readTockens();
	return 0;
}