#include <iostream>
#include <cctype>
#include <vector>
#include <map>
#include <string>
#include <fstream>

using namespace std;

enum  Const {
	NUM, DNUM, STR,
	ID,
	IF, ELIF, ELSE, WHILE, BREAK, CONTINUE,
	LPAR, RPAR, LBR, RBR, LBRA, RBRA,
	PLUS, MINUS, MULTIPLY, DIVIDE, PLUSEQUAL, MINUSEQUAL, MULTIPLYEQUAL, DIVIDEEQUAL,
	LESS, GREATER, EQUAL, LESSEQUAL, GREATEREQUAL, NOT, NOTEQUAL, EQUALEQUAL,
	COLON, EF, QUOTE, COMMA, SEMICOLON,
	PRINT, MAX, AND, LEN, INPUT, INT,
	FOR, IN, RANGE,
	DEF, RETURN, ERR
};

struct Tocken {
	Const symb;
	string value;
};

class Lexer {
private:
	

	std::map<char, Const> symbols = {
		{'=', EQUAL},
		{':', COLON},
		{'(', LPAR},
		{')', RPAR},
		{'[', LBR},
		{']', RBR},
		{'{', LBRA},
		{'}', RBRA},
		{'+', PLUS},
		{'-', MINUS},
		{'*', MULTIPLY},
		{'/', DIVIDE},
		{'<', LESS},
		{'>', GREATER},
		{'!', NOT},
		{'"', QUOTE},
		{'\'', QUOTE},
		{',', COMMA},
		{';', SEMICOLON}
	};

	std::map<string, Const> words = { 
		{"if", IF},
		{"elif", ELIF},
		{"else", ELSE},
		{"while", WHILE},
		{"print", PRINT},
		{"max", MAX},
		{"for", FOR},
		{"in", IN},
		{"range", RANGE},
		{"break", BREAK},
		{"continue", CONTINUE},
		{"and", AND},
		{"len", LEN},
		{"int", INT},
		{"input", INPUT},
		{"def", DEF}
	};

	char currentSymbol;

	string fileName;
	int lineCounter;
	int symbolCounter;

	int parenthesesCounter = 0;

	bool isString = false;
	bool isU = false;

	ifstream readFile;

	void getNextChar() {
		readFile.get(currentSymbol);
		symbolCounter += 1;

		this->removeComment();

		if (readFile.eof()) {
			currentSymbol = '\0';
		}
	}
	void showError(string message) {
		printf("%s:%d:%d:error: %s\n", fileName.c_str(), lineCounter, symbolCounter, message.c_str());
	}
	void removeComment() {
		if (currentSymbol == '#') {
			string str;
			getline(readFile, str);

			lineCounter += 1;
			symbolCounter = 0;

			getNextChar();
		}
	}

	string checkParentless() {
		if (parenthesesCounter != 0) {
			string err = "Missmatched Parentheses";
			this->showError(err);
			return err;
		}
		parenthesesCounter = 0;

		return "";
	}
	
public:
	Lexer() {}
	Lexer(string fileName) {
		readFile.open(fileName);

		lineCounter = 1;
		symbolCounter = 0;
		this->fileName = fileName;

		getNextChar();
	}

	Lexer(Lexer& lex) {
		this->currentSymbol = lex.currentSymbol;
		this->fileName = lex.fileName;
		this->isString = lex.isString;
		this->isU = lex.isU;
		this->lineCounter = lex.lineCounter;
		this->parenthesesCounter = lex.parenthesesCounter;
		this->symbolCounter = lex.symbolCounter;
		this->symbols = lex.symbols;
		this->words = lex.words;
	}

	Lexer& operator=(const Lexer& lex) {
		return *this;
	}
	
	Tocken getNextTocken() {
		Tocken tocken;
		
		while (currentSymbol == ' ' || currentSymbol == '\n' || currentSymbol == '\t') {
			if (currentSymbol == '\n') {
				string check = checkParentless();
				if (check != "") {
					return Tocken(ERR, check);
				}
				lineCounter += 1;
				symbolCounter = 0;
			}
			getNextChar();
		}
		
		if (readFile.eof() || currentSymbol == '\0') {
			tocken.symb = EF;
			return tocken;
		}
		else if (isdigit(currentSymbol) || currentSymbol == '-') {
			tocken.value = "";
			if (currentSymbol == '-') {
				getNextChar();
				if (!isdigit(currentSymbol)) {
					tocken.symb = symbols[currentSymbol];
					getNextChar();

					return tocken;
				}
				tocken.value += '-';
			}
			while (isdigit(currentSymbol) && currentSymbol != '\0') {
				tocken.value += currentSymbol;
				getNextChar();
			}

			if (currentSymbol == '.') {
				tocken.value += currentSymbol;
				tocken.symb = DNUM;
				getNextChar();
			}
			else if (currentSymbol == ' ' || currentSymbol == '\0' || currentSymbol == '\n' || (symbols.find(currentSymbol) != symbols.end())) {
				tocken.symb = NUM;

				return tocken;
			}
			else {
				string err = "Syntax error near ";
				err += tocken.value;

				err += " unexpected symbol ";
				err += currentSymbol;

				this->showError(err);

				tocken.symb = ERR;
				tocken.value = err;

				getNextChar();

				return tocken;
			}

			while (isdigit(currentSymbol)) {
				tocken.value += currentSymbol;
				getNextChar();
			}

			return tocken;
		}
		else if (!(symbols.find(currentSymbol) == symbols.end())) {
			tocken.symb = symbols[currentSymbol];
			getNextChar();

			if (tocken.symb == GREATER && this->currentSymbol == '=') {
				tocken.symb = GREATEREQUAL;
				getNextChar();
			}
			else if (tocken.symb == LESS && this->currentSymbol == '=') {
				tocken.symb = LESSEQUAL;
				getNextChar();
			}
			else if (tocken.symb == NOT && this->currentSymbol == '=') {
				tocken.symb = NOTEQUAL;
				getNextChar();
			}
			else if (tocken.symb == EQUAL && this->currentSymbol == '=') {
				tocken.symb = EQUALEQUAL;
				getNextChar();
			}
			else if (tocken.symb == PLUS && this->currentSymbol == '=') {
				tocken.symb = PLUSEQUAL;
				getNextChar();
			}
			else if (tocken.symb == MINUS && this->currentSymbol == '=') {
				tocken.symb = MINUSEQUAL;
				getNextChar();
			}
			else if (tocken.symb == MULTIPLY && this->currentSymbol == '=') {
				tocken.symb = MULTIPLYEQUAL;
				getNextChar();
			}
			else if (tocken.symb == DIVIDE && this->currentSymbol == '=') {
				tocken.symb = DIVIDEEQUAL;
				getNextChar();
			}

			if (tocken.symb == QUOTE) {
				isString = !isString;
				if (isString) {
					isU = false;
				}
			}

			if (tocken.symb == LPAR) {
				parenthesesCounter += 1;
			}
			else if (tocken.symb == RPAR) {
				parenthesesCounter -= 1;
			}

			return tocken;

		}
		else if (isalpha(currentSymbol)) {
			string word = "";
			bool isSpecialWord = false;
			while ((!isString && (isalpha(currentSymbol) || isdigit(currentSymbol) || currentSymbol == '_')) || (isString && (currentSymbol != '"' && currentSymbol != '\''))) {
				word += tolower(currentSymbol);

				if (currentSymbol == 'u') {
					isU = true;
				}
				getNextChar();

				if (!(words.find(word) == words.end())) {
					if (!isString && (isalpha(currentSymbol) || isdigit(currentSymbol) || currentSymbol == '_')) {
						continue;
					}
					tocken.symb = words[word];
					isSpecialWord = true;

					return tocken;
				}

				if (isString && currentSymbol == '\n') {
					string err = "Expected \" or ' but found ";
					err += this->currentSymbol;

					this->showError(err);

					tocken.symb = ERR;
					tocken.value = err;

					getNextChar();

					return tocken;
				}

			}
			if (isString) {
				tocken.symb = STR;
				tocken.value = word;

				return tocken;
			}
			else if (!isString && !isSpecialWord) {
				tocken.symb = ID;
				tocken.value = word;

				return tocken;
			}
		}
		else {
			string err = "Unexpected symbol: ";
			err += this->currentSymbol;

			this->showError(err);

			tocken.symb = ERR;
			tocken.value = err;

			getNextChar();
			
			return tocken;
		}
	}

	void readTockens() {
		Tocken tocken;
		tocken = getNextTocken();

		while (tocken.symb != EF && tocken.symb != ERR) {
			cout << tocken.symb << " " << tocken.value << endl;
			tocken = getNextTocken();
		}
	}

	~Lexer() {
		readFile.close();
	}
};