#include <iostream>
#include <vector>
#include <string>
#include "Parser.cpp"

using namespace std;

int main() {
	Parser parser = Parser("script.txt");
	parser.Parse();

	return 0;
}