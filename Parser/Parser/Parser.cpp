#include <iostream>
#include <vector>
#include <map>
#include "Lexer.cpp"

using namespace std;

class Data {
protected: 
	enum  Kind {
		VAR, CONSTN, CONSTD, CONSTS,
		ADD, SUB, MULTIPLY, DIVIDE,
		PLUSEQ, SUBEQ, MULTEQ, DIVEQ,
		LT, LE, GT, GE, EQ, NEQ,
		SET, RANGE,
		IF, ELIF, ELSE,
		WHILE, FOR,
		EMPTY, SEQ, PROG,
		INT, PRINT, INPUT, MAX,
		EXPR
	};
	vector<Kind> types = { CONSTN, CONSTD, CONSTS};
	vector<Kind> rules = { LT, LE, GT, GE, EQ, NEQ };
	vector<Kind> operations = { ADD, SUB, MULTIPLY, DIVIDE};
	vector<Kind> operationsEq = { PLUSEQ, SUBEQ, MULTEQ, DIVEQ };

	std::map<Kind, string> symbolsKind = {
	{VAR, ""},
	{CONSTN, "INT"},
	{CONSTD, "FLOAT"},
	{CONSTS, "STR"},
	{ADD, "+"},
	{SUB, "-"},
	{MULTIPLY, "*"},
	{DIVIDE, "/"},
	{LT, "<"},
	{LE, "<="},
	{GT, ">"},
	{GE, ">="},
	{EQ, "="},
	{SET, "="},
	{RANGE, "RANGE"},
	{IF, "IF"},
	{ELIF, "ELIF"},
	{ELSE, "ELSE"},
	{WHILE, "WHILE"},
	{FOR, "FOR"},
	{EMPTY, "EMPTY"},
	{SEQ, "SEQ"},
	{MAX, "MAX"},
	{PROG, "PROG"},
	{INT, "INT"},
	{PRINT, "PRINT"},
	{INPUT, "INPUT"},
	{EXPR, ""},
	};
	struct Node {
		vector<Node*> op;
		string value;
		Kind kind;
	};
};

class Parser: public Data {
private:
	map<string, Kind> variables;
	int currentTab = 0;
	Lexer lexer;
	Tocken currTocken;

public:
	Parser() {}
	Parser(string filename) {
		lexer = Lexer(filename);
	}
	Parser(const Parser& parser) {
		this->currTocken = parser.currTocken;
		this->lexer = parser.lexer;
		this->symbolsKind = parser.symbolsKind;
		this->variables = parser.variables;
		this->tabs = parser.tabs;
	}

	Parser& operator=(const Parser& parser) {
		if (this == &parser) {
			return *this;
		}

		this->currTocken = parser.currTocken;
		this->lexer = parser.lexer;
		this->symbolsKind = parser.symbolsKind;
		this->variables = parser.variables;
		this->tabs = parser.tabs;

		return *this;
	}
	Node* createNode(Kind kind, Node* op1) {
		Node* newNode = new Node();
		newNode->kind = kind;
		newNode->op.push_back(op1);
		return newNode;
	}
	Node* term() {
		if (this->currTocken.symb == Const::INT || this->currTocken.symb == Const::PRINT || this->currTocken.symb == Const::INPUT || this->currTocken.symb == Const::MAX) {
			return this->parseFunctions();
		} else if (this->currTocken.symb == Const::ID) {
			Node* nodeId = new Node();

			nodeId->kind = VAR;
			nodeId->value = currTocken.value;

			this->currTocken = this->lexer.getNextTocken();

			return nodeId;
		} else if (this->currTocken.symb == Const::NUM) {
			Node* nodeNum = new Node();
			nodeNum->kind = CONSTN;
			nodeNum->value = this->currTocken.value;

			this->currTocken = this->lexer.getNextTocken();

			return nodeNum;
		} else if (this->currTocken.symb == Const::DNUM) {
			Node* nodeDoubleNum = new Node();
			nodeDoubleNum->kind = CONSTD;
			nodeDoubleNum->value = this->currTocken.value;

			this->currTocken = this->lexer.getNextTocken();

			return nodeDoubleNum;
		} else if (this->currTocken.symb == Const::STR) {
			Node* nodeStr = new Node();
			nodeStr->kind = CONSTS;
			nodeStr->value = this->currTocken.value;

			this->currTocken = this->lexer.getNextTocken();

			return nodeStr;
		} else {
			return this->parseParentExpression();
		}
	}
	
	Node* parseFunctions() {
		if (this->currTocken.symb == Const::INT) {
			Node* nodeInt = new Node();
			
			nodeInt->kind = INT;
			this->currTocken = this->lexer.getNextTocken();

			nodeInt->op.push_back(this->parseParentExpression());

			return nodeInt;

		} else if (this->currTocken.symb == Const::PRINT) {
			Node* nodePrint = new Node();
			nodePrint->kind = PRINT;

			this->currTocken = this->lexer.getNextTocken();

			nodePrint->op.push_back(this->parseParentExpression());

			return nodePrint;

		} else if (this->currTocken.symb == Const::INPUT) {
			Node* nodeInput = new Node();
			nodeInput->kind = INPUT;

			this->currTocken = this->lexer.getNextTocken();

			nodeInput->op.push_back(this->parseParentExpression());

			return nodeInput;
		} else if (this->currTocken.symb == Const::MAX) {
			Node* nodeMAX = new Node();
			nodeMAX->kind = MAX;

			this->currTocken = this->lexer.getNextTocken();

			if (this->currTocken.symb != Const::LPAR) {
				this->showError("SYNTAX ERROR '(' EXPECTED!");
			}

			this->currTocken = this->lexer.getNextTocken();
			nodeMAX->op.push_back(this->parseArithmeticExpression());

			if (this->currTocken.symb != Const::COMMA) {
				this->showError("EXPECTED COMMA IN MAX EXPRESSION");
			}
			this->currTocken = this->lexer.getNextTocken();
			nodeMAX->op.push_back(this->parseArithmeticExpression());

			if (this->currTocken.symb != Const::RPAR) {
				this->showError("SYNTAX ERROR ')' EXPECTED!");
			}
			this->currTocken = this->lexer.getNextTocken();
			
			if (variables[nodeMAX->op[0]->value] != variables[nodeMAX->op[1]->value]) {
				this->showError("Operator max must be with similar types, but found " + symbolsKind[variables[nodeMAX->op[0]->value]] + " and " + symbolsKind[variables[nodeMAX->op[1]->value]]);
			}
			return nodeMAX;
		}
	}
	
	Node* parseArithmeticExpression() {
		Node* node = this->term();

		if (this->currTocken.symb == Const::PLUS || this->currTocken.symb == Const::MINUS ||
			this->currTocken.symb == Const::MULTIPLY || this->currTocken.symb == Const::DIVIDE) {
			Node* arithmeticExpression = new Node();
			arithmeticExpression->kind = EXPR;
			arithmeticExpression->op.push_back(node);

			Kind kindTemp;
			while (this->currTocken.symb == Const::PLUS || this->currTocken.symb == Const::MINUS ||
				this->currTocken.symb == Const::MULTIPLY || this->currTocken.symb == Const::DIVIDE) {
				switch (this->currTocken.symb) {
				case Const::PLUS:
					kindTemp = ADD;
					break;
				case Const::MINUS:
					kindTemp = SUB;
					break;
				case Const::MULTIPLY:
					kindTemp = MULTIPLY;
					break;
				case Const::DIVIDE:
					kindTemp = DIVIDE;
					break;
				}

				this->currTocken = this->lexer.getNextTocken();

				Node* n = new Node();
				n->kind = kindTemp;
				n->op.push_back(this->term());
				arithmeticExpression->op.push_back(n);
			}
			return arithmeticExpression;
		}
		
		return node;
	}

	Node* parseCompareExpression() {
		this->parseQuote();
		Node* n = new Node();
		n = this->parseArithmeticExpression();
		this->parseQuote();
		if (this->currTocken.symb == Const::LESS) {
			this->currTocken = this->lexer.getNextTocken();
			Node* nodeComp = new Node();
			nodeComp->kind = LT;
			nodeComp->op.push_back(this->parseArithmeticExpression());
			n->op.push_back(nodeComp);
		} else if (this->currTocken.symb == Const::GREATER) {
			this->currTocken = this->lexer.getNextTocken();
			Node* nodeComp = new Node();
			nodeComp->kind = GT;
			nodeComp->op.push_back(this->parseArithmeticExpression());
			n->op.push_back(nodeComp);
		} else if (this->currTocken.symb == Const::GREATEREQUAL) {
			this->currTocken = this->lexer.getNextTocken();
			Node* nodeComp = new Node();
			nodeComp->kind = GE;
			nodeComp->op.push_back(this->parseArithmeticExpression());
			n->op.push_back(nodeComp);
		} else if (this->currTocken.symb == Const::LESSEQUAL) {
			this->currTocken = this->lexer.getNextTocken();
			Node* nodeComp = new Node();
			nodeComp->kind = LE;
			nodeComp->op.push_back(this->parseArithmeticExpression());
			n->op.push_back(nodeComp);
		} else if (this->currTocken.symb == Const::EQUALEQUAL) {
			this->currTocken = this->lexer.getNextTocken();
			Node* nodeComp = new Node();
			nodeComp->kind = EQ;
			nodeComp->op.push_back(this->parseArithmeticExpression());
			n->op.push_back(nodeComp);
		}	else if (this->currTocken.symb == Const::NOTEQUAL) {
			this->currTocken = this->lexer.getNextTocken();
			Node* nodeComp = new Node();
			nodeComp->kind = NEQ;
			nodeComp->op.push_back(this->parseArithmeticExpression());
			n->op.push_back(nodeComp);
		} else if (this->currTocken.symb == Const::PLUSEQUAL) {
			this->currTocken = this->lexer.getNextTocken();
			Node* nodePlusEq = new Node();
			nodePlusEq->kind = PLUSEQ;
			nodePlusEq->op.push_back(this->parseArithmeticExpression());
			if (n->kind == VAR && variables[n->value] != nodePlusEq->op[0]->kind) {
				this->showError("Operator += must be with similar types, but found " + symbolsKind[variables[n->value]] + " and " + symbolsKind[nodePlusEq->op[0]->kind]);
			}
			n->op.push_back(nodePlusEq);
		} else if (this->currTocken.symb == Const::MINUSEQUAL) {
			this->currTocken = this->lexer.getNextTocken();
			Node* nodePlusEq = new Node();
			nodePlusEq->kind = Kind::SUBEQ;
			nodePlusEq->op.push_back(this->parseArithmeticExpression());
			if (n->kind == VAR && variables[n->value] != nodePlusEq->op[0]->kind) {
				this->showError("Operator -= must be with similar types, but found " + symbolsKind[variables[n->value]] + " and " + symbolsKind[nodePlusEq->op[0]->kind]);
			}
			n->op.push_back(nodePlusEq);
		} else if (this->currTocken.symb == Const::MULTIPLYEQUAL) {
			this->currTocken = this->lexer.getNextTocken();
			Node* nodePlusEq = new Node();
			nodePlusEq->kind = Kind::MULTEQ;
			nodePlusEq->op.push_back(this->parseArithmeticExpression());
			if (n->kind == VAR && variables[n->value] != nodePlusEq->op[0]->kind) {
				this->showError("Operator *= must be with similar types, but found " + symbolsKind[variables[n->value]] + " and " + symbolsKind[nodePlusEq->op[0]->kind]);
			}
			n->op.push_back(nodePlusEq);
		} else if (this->currTocken.symb == Const::DIVIDEEQUAL) {
			this->currTocken = this->lexer.getNextTocken();
			Node* nodePlusEq = new Node();
			nodePlusEq->kind = Kind::DIVEQ;
			nodePlusEq->op.push_back(this->parseArithmeticExpression());
			if (n->kind == VAR && variables[n->value] != nodePlusEq->op[0]->kind) {
				this->showError("Operator /= must be with similar types, but found " + symbolsKind[variables[n->value]] + " and " + symbolsKind[nodePlusEq->op[0]->kind]);
			}
			n->op.push_back(nodePlusEq);
		}

		return n;
	}

	Node* parseExpression() {
		
		if (this->currTocken.symb != Const::ID) {
			return this->parseCompareExpression();
		}

		Node* n = this->parseCompareExpression();

		if (n->op.size() > 0 && n->kind == Kind::EXPR) {
			for (int i = 0; i < n->op.size(); i++) {
				if ((n->op[i])->kind == Kind::ADD || (n->op[i])->kind == Kind::SUB
					|| (n->op[i])->kind == Kind::MULTIPLY || (n->op[i])->kind == Kind::DIVIDE) {
					for (int j = 0; j < ((n->op[i])->op).size(); j++) {
						if (((n->op[i])->op[j])->kind != Kind::VAR && ((n->op[i])->op[j])->kind != variables[n->op[0]->value]) {
							this->showError("Operation " + symbolsKind[(n->op[i])->kind] + " unavailiable between " + symbolsKind[((n->op[i])->op[j])->kind] + " and " + symbolsKind[variables[n->op[0]->value]]);
						}
						
						else if (((n->op[i])->op[j])->kind == Kind::VAR && variables[((n->op[i])->op[j])->value] != variables[n->op[0]->value]) {
							this->showError("Operation " + symbolsKind[(n->op[i])->kind] + " unavailiable between " + symbolsKind[variables[((n->op[i])->op[j])->value]] + " and " + symbolsKind[variables[n->op[0]->value]]);
						}
					}
				}
			}
		}

		if (n->kind == VAR && this->currTocken.symb == Const::EQUAL) {
			this->currTocken = this->lexer.getNextTocken();

			Node* nodeExpr = new Node();

			nodeExpr->kind = SET;
			nodeExpr->op.push_back(this->parseExpression());
			if (nodeExpr->op.size() > 0 && nodeExpr->op[0]->kind == Kind::INT) {
				variables.insert({ n->value, Kind::CONSTN });
			} else if (nodeExpr->op.size() > 0 && nodeExpr->op[0]->kind == Kind::INPUT) {
				variables.insert({ n->value, Kind::CONSTS });
			} else if (nodeExpr->op.size() > 0 && (nodeExpr->op[0]->kind == Kind::CONSTD || nodeExpr->op[0]->kind == Kind::CONSTN || nodeExpr->op[0]->kind == Kind::CONSTS)) {
				variables.insert({ n->value, nodeExpr->op[0]->kind });
			}

			n->op.push_back(nodeExpr);
		}
		return n;
	}

	void parseQuote() {
		if (this->currTocken.symb == Const::QUOTE) {
			this->currTocken = this->lexer.getNextTocken();
		}
	}

	Node* parseParentExpression() {
		if (this->currTocken.symb != Const::LPAR) {
			this->showError("SYNTAX ERROR '(' EXPECTED!");
		}
		this->currTocken = this->lexer.getNextTocken();
		this->parseQuote();
		Node* n = this->parseExpression();
		
		this->parseQuote();

		if (this->currTocken.symb != Const::RPAR) {
			this->showError("SYNTAX ERROR ')' EXPECTED!");
		}
		this->currTocken = this->lexer.getNextTocken();

		return n;
	}
	Node* parseStatementFor() {
		if (this->currTocken.symb != Const::RANGE) {
			showError("Syntax error! Operator range expected!");
		}
		Node* nodeFor = new Node();
		nodeFor->kind = RANGE;

		this->currTocken = this->lexer.getNextTocken();

		if(this->currTocken.symb != Const::LPAR) {
			this->showError("SYNTAX ERROR '(' EXPECTED!");
		}
		this->currTocken = this->lexer.getNextTocken();

		Node* n = this->parseExpression();
		nodeFor->op.push_back(n);

		if (this->currTocken.symb == Const::COMMA) {
			this->currTocken = this->lexer.getNextTocken();
			Node* n = this->parseExpression();
			nodeFor->op.push_back(n);
		}

		if (this->currTocken.symb != Const::RPAR) {
			this->showError("SYNTAX ERROR ')' EXPECTED!");
		}

		this->currTocken = this->lexer.getNextTocken();

		return nodeFor;
	}

	Node* parseStatement() {
		Node* n = new Node();
		if (this->currTocken.symb == Const::IF) {
			n->kind = Kind::IF;
			//if node
			//Node* ifNode = new Node();
			n->kind = Kind::IF;

			this->currTocken = this->lexer.getNextTocken();

			//parse expression
			if (this->currTocken.symb == Const::LPAR) {
				n->op.push_back(this->parseParentExpression());
			}
			else {
				n->op.push_back(this->parseExpression());
			}
			//checks if a colon is present
			if (this->currTocken.symb != Const::COLON) {
				this->showError("EXPECTED COLON AFTER IF STATEMENT!");
			}
			if (this->currentTab != 0) {
				n->op.push_back(this->parseStatement());
				this->currentTab = 0;
			}
			//parse statement
			this->currTocken = this->lexer.getNextTocken();
			int counter = 0;
			int tabCount = 0;
			while (true) {
				while (this->currTocken.symb == Const::TAB) {
					this->currTocken = this->lexer.getNextTocken();
					counter++;
				}
				if(tabCount==0) tabCount = counter;
				if (tabCount != counter) {
					this->currentTab = counter;
					break;
				}
				n->op.push_back(this->parseStatement());
				if (counter == 0) break;
				counter = 0;
			}
			//append to node
			//n->op.push_back(n);
		} else if(this->currTocken.symb == Const::ELIF){
			//same for  (might be more than one)
			while (this->currTocken.symb == Const::ELIF) {
				//creating node for elif
				//Node* elifNode = new Node();
				n->kind = Kind::ELIF;

				this->currTocken = this->lexer.getNextTocken();

				//parse expression
				if (this->currTocken.symb == Const::LPAR) {
					n->op.push_back(this->parseParentExpression());
				}
				else {
					n->op.push_back(this->parseExpression());
				}

				//checks if a colon is present
				if (this->currTocken.symb != Const::COLON) {
					this->showError("EXPECTED COLON AFTER IF STATEMENT!");
				}

				//parse statement
				this->currTocken = this->lexer.getNextTocken();

			/*	while (this->currTocken.symb == Const::TAB) {
					this->currTocken = this->lexer.getNextTocken();
					n->op.push_back(this->parseStatement());
				}*/
				if (this->currentTab != 0) {
					n->op.push_back(this->parseStatement());
					this->currentTab = 0;
				}
				int counter = 0;
				int tabCount = 0;
				while (true) {
					while (this->currTocken.symb == Const::TAB) {
						this->currTocken = this->lexer.getNextTocken();
						counter++;
					}
					if (tabCount == 0) tabCount = counter;
					if (tabCount != counter) {
						this->currentTab = counter;
						break;
					}
					n->op.push_back(this->parseStatement());
					if (counter == 0) break;
					counter = 0;
				}
				//append to node
				//n->op.push_back(n);
			}
			//same for else
		}
		else if (this->currTocken.symb == Const::ELSE) {
				//new node for else
				//Node* elseNode = new Node();
				n->kind = Kind::ELSE;

				this->currTocken = this->lexer.getNextTocken();
				//checks if a colon is present
				if (this->currTocken.symb != Const::COLON) {
					this->showError("EXPECTED COLON AFTER IF STATEMENT!");
				}
				//parse statement
				//this->currTocken = this->lexer.getNextTocken();
				
				/*while (this->currTocken.symb == Const::TAB) {
					this->currTocken = this->lexer.getNextTocken();
					n->op.push_back(this->parseStatement());
				}*/
				//parse statement
				this->currTocken = this->lexer.getNextTocken();
				int counter = 0;
				int tabCount = 0;
				while (true) {
					while (this->currTocken.symb == Const::TAB) {
						this->currTocken = this->lexer.getNextTocken();
						counter++;
					}
					if (tabCount == 0) tabCount = counter;
					if (tabCount != counter) {
						this->currentTab = counter;
						break;
					}
					n->op.push_back(this->parseStatement());
					if (this->currentTab == counter) {
						n->op.push_back(this->parseStatement());
						this->currentTab = 0;
					}
					if (counter == 0) break;
					counter = 0;
				}
				//append to node
				//n->op.push_back(elseNode);
			}
		else if (this->currTocken.symb == Const::WHILE) {
			n->kind = Kind::WHILE;
			this->currTocken = this->lexer.getNextTocken();

			//parse expression(might be in brackets)
			if (currTocken.symb == Const::LPAR) {
				n->op.push_back(this->parseParentExpression());
			}
			else {
				n->op.push_back(this->parseExpression());
			}

			//checks if a colon is present
			if (this->currTocken.symb != Const::COLON) {
				this->showError("EXPECTED COLON AFTER IF STATEMENT!");
			}
			//this->currTocken = this->lexer.getNextTocken();
			//
			/*while (this->currTocken.symb == Const::TAB) {
				this->currTocken = this->lexer.getNextTocken();
				n->op.push_back(this->parseStatement());
			}*/
			if (this->currentTab != 0) {
				n->op.push_back(this->parseStatement());
				this->currentTab = 0;
			}
			//parse statement
			this->currTocken = this->lexer.getNextTocken();
			int counter = 0;
			int tabCount = 0;
			while (true) {
				while (this->currTocken.symb == Const::TAB) {
					this->currTocken = this->lexer.getNextTocken();
					counter++;
				}
				if (tabCount == 0) tabCount = counter;
				while (tabCount != counter) {
					this->currentTab = counter;
					n->op.push_back(this->parseStatement());
					break;
				}
				n->op.push_back(this->parseStatement());
				if (counter == 0) break;
				counter = 0;
			}
		}
		else if (this->currTocken.symb == Const::FOR) {
			n->kind = Kind::FOR;
			this->currTocken = this->lexer.getNextTocken();
			//cannot be in brackets
			//maybe there shoud be different parse function for expr
			n->op.push_back(this->term());

			variables.insert({ n->op[0]->value, Kind::CONSTN });
			if (this->currTocken.symb != Const::IN) {
				showError("Syntax error. Operator IN expected!");
			}
			this->currTocken = this->lexer.getNextTocken();

			n->op.push_back(this->parseStatementFor());

			//this->currTocken = this->lexer.getNextTocken();

			/*while (this->currTocken.symb == Const::TAB) {
				this->currTocken = this->lexer.getNextTocken();
				n->op.push_back(this->parseStatement());
			}*/
			if (this->currentTab != 0) {
				n->op.push_back(this->parseStatement());
				this->currentTab = 0;
			}
			//parse statement
			this->currTocken = this->lexer.getNextTocken();
			int counter = 0;
			int tabCount = 0;
			while (true) {
				while (this->currTocken.symb == Const::TAB) {
					this->currTocken = this->lexer.getNextTocken();
					counter++;
				}
				if (tabCount == 0) tabCount = counter;
				if (tabCount != counter) {
					this->currentTab = counter;
					break;
				}
				n->op.push_back(this->parseStatement());
				if (counter == 0) break;
				counter = 0;
			}

		}
		else if (this->currTocken.symb == Const::SEMICOLON) {
			//; do nothing
			n->kind = EMPTY;
			this->currTocken = this->lexer.getNextTocken();
		}
		else if (this->currTocken.symb == Const::LBRA) {
			// block { }
			n->kind = EMPTY;
			this->currTocken = this->lexer.getNextTocken();
			while (this->currTocken.symb != Const::RBRA) {
				n->kind = SEQ;
				n->op.push_back(n);
				n->op.push_back(parseStatement());
			}
			this->currTocken = this->lexer.getNextTocken();
			while (this->currTocken.symb == Const::TAB) {
				this->currTocken = this->lexer.getNextTocken();
			}
			n->op.push_back(this->parseStatement());
		}
		else {
			n->kind = EXPR;
			if (this->currTocken.symb == Const::TAB) {
				while (this->currTocken.symb == Const::TAB) {
					this->currTocken = this->lexer.getNextTocken();
				}
			}
			n->op.push_back(this->parseExpression());
		}
		return n;
	}

	Node* Parse() {
		currTocken = this->lexer.getNextTocken();

		Node* node = createNode(PROG, this->parseStatement());
		
		while (this->currTocken.symb != Const::EF) {
			node->op.push_back(this->parseStatement());
		}
		
		return node;
	}
	string tabs = "\t";
	void showTree(Node* root) {
		if (root->value != "") {
			cout << tabs << root->value << endl;
		}
		for (int i = 0; i < root->op.size(); i++) {
			cout << tabs << symbolsKind[root->kind] << endl;
			tabs += '\t';
			if (root->op.size() == 0) {
				
			}
			showTree(root->op[i]);
			tabs.pop_back();
		}
	}
	void showError(string message) {
		cout << "Error in "<< lexer.getLine() << " line, " <<lexer.getSymbolCounter() << " symbol. " << message << endl;
		exit(-1);
	}

	int getLine() {
		return lexer.getLine();
	}

	int getSymbolCounter() {
		return lexer.getSymbolCounter();
	}

	Kind getType(string var) {
		return variables[var];
	}

};
