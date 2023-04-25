#include <iostream>
#include <vector>
#include <string>
#include "Compiler.cpp"

using namespace std;

int main() {
	Compiler compilator = Compiler("script2.txt");
	compilator.compile();

	return 0;
}