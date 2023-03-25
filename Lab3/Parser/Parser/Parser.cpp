#include <iostream>
#include <vector>
#include "Lexer.cpp"

using namespace std;

class Parser {
private:
	enum  Kind {
		VAR, CONST,
		ADD, SUB,
		LT,
		SET,
		IF, ELIF, ELSE,
		WHILE, DO, FOR,
		EMPTY, SEQ, PROG,
		EXPR
	};

	struct Node {
		vector<Node*> op;
		string value;
		Kind kind;
	};

	Lexer lexer;
	Tocken currTocken;
public: 
	Parser(string filename) {
		this->lexer = Lexer(filename);
	}
	Node* createNode(Kind kind, Node* op1) {
		Node* newNode = new Node();
		newNode->kind = kind;
		newNode->op.push_back(op1);
		return newNode;
	}
	Node* parseExpression() {
	
	}

	Node* parseParentExpression() {
		if (this->currTocken.symb != Const::LPAR) {
			this->showError("SYNTAX ERROR '(' EXPECTED!");
		}
		this->currTocken = this->lexer.getNextTocken();

		Node* n = this->parseExpression();

		if (this->currTocken.symb != Const::RPAR) {
			this->showError("SYNTAX ERROR ')' EXPECTED!");
		}
		this->currTocken = this->lexer.getNextTocken();

		return n;
	}

	Node* parseStatement() {
		Node* n = new Node();
		if (this->currTocken.symb == Const::IF) {
			n->kind = Kind::IF;
			//if node
			Node* ifNode = new Node();
			ifNode->kind = Kind::IF;

			this->currTocken = this->lexer.getNextTocken();

			//parse expression
			if (this->currTocken.symb == Const::LPAR) {
				ifNode->op.push_back(this->parseParentExpression());
			} else {
				ifNode->op.push_back(this->parseExpression());
			}
			//checks if a colon is present
			if (this->currTocken.symb == Const::COLON) {
				this->showError("EXPECTED COLON AFTER IF STATEMENT!");
			}

			//parse statement
			this->currTocken = this->lexer.getNextTocken();
			ifNode->op.push_back(this->parseStatement());

			//append to node
			n->op.push_back(ifNode);

			//same for  (might be more than one)
			while (this->currTocken.symb == Const::ELIF) {
				//creating node for elif
				Node* elifNode = new Node();
				elifNode->kind = Kind::ELIF;

				this->currTocken = this->lexer.getNextTocken();

				//parse expression
				if (this->currTocken.symb == Const::LPAR) {
					elifNode->op.push_back(this->parseParentExpression());
				}
				else {
					elifNode->op.push_back(this->parseExpression());
				}

				//checks if a colon is present
				if (this->currTocken.symb == Const::COLON) {
					this->showError("EXPECTED COLON AFTER IF STATEMENT!");
				}

				//parse statement
				this->currTocken = this->lexer.getNextTocken();
				elifNode->op.push_back(this->parseStatement());

				//append to node
				n->op.push_back(elifNode);
			}
			//same for else
			if (this->currTocken.symb == Const::ELSE) {
				//new node for else
				Node* elseNode = new Node();
				elseNode->kind = Kind::ELSE;

				this->currTocken = this->lexer.getNextTocken();
				//checks if a colon is present
				if (this->currTocken.symb == Const::COLON) {
					this->showError("EXPECTED COLON AFTER IF STATEMENT!");
				}
				//parse statement
				this->currTocken = this->lexer.getNextTocken();
				elseNode->op.push_back(this->parseStatement());

				//append to node
				n->op.push_back(elseNode);
			}

		} else if (this->currTocken.symb == Const::WHILE) {
			n->kind = Kind::WHILE;
			this->currTocken = this->lexer.getNextTocken();
			
			//parse expression(might be in brackets)
			if (currTocken.symb == Const::LPAR) {
				n->op.push_back(this->parseParentExpression());
			} else {
				n->op.push_back(this->parseExpression());
			}

			n->op.push_back(this->parseStatement());
		} else if (this->currTocken.symb == Const::FOR) {
			n->kind = Kind::FOR;
			this->currTocken = this->lexer.getNextTocken();
			//cannot be in brackets
			//maybe there shoud be different parse function for expr
			n->op.push_back(this->parseExpression());
			n->op.push_back(this->parseStatement());
		} else if (this->currTocken.symb == Const::SEMICOLON) {
			//; do nothing
			n->kind = EMPTY;
			this->currTocken = this->lexer.getNextTocken();
		} else if (this->currTocken.symb == Const::LBRA) {
			// block { }
			n->kind = EMPTY;
			this->currTocken = this->lexer.getNextTocken();
			while (this->currTocken.symb != Const::RBRA) {
				n->kind = SEQ;
				n->op.push_back(n);
				n->op.push_back(parseStatement());
			}
			this->currTocken = this->lexer.getNextTocken();
		} else {
			n->kind = EXPR;
			n->op.push_back(this->parseExpression());
			this->currTocken = this->lexer.getNextTocken();
		}
	
	}

	Node* Parse() {
		currTocken = this->lexer.getNextTocken();
		Node* node = createNode(PROG, this->parseStatement());
		if (this->currTocken.symb != EOF) {
			this->showError("Invalid statement syntax");
		}
		return node;
	}

	void showError(string message) {
		cout << message << endl;
		exit(-1);
	}

};
