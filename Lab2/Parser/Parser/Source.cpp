#include <iostream>
#include <vector>
#include <string>
#include "Parser.cpp"

using namespace std;

int main() {
	Parser parser = Parser("script.txt");
	parser.Parse();
	//Lexer lexer = Lexer("script.txt");
	//lexer.readTockens();

	return 0;
}