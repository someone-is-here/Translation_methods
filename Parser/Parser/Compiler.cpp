#include <iostream>
#include <map>
#include <vector>
#include "Parser.cpp"

using namespace std;

class Compiler : public Data {
private:
	Parser parser;
	map<string, string> valuesString;
	map<string, int> valuesNumber;
	map<string, vector<int>> valuesRange;
	map<string, double> valuesFloat;
	int max_value;
public:
	Compiler(string filename) {
		parser = Parser(filename);
	}
	int my_stoi(string value) {
		int res = 0;
		try {
			res = stoi(value);
			return res;
		}
		catch (...) {
			cout << "Impossible to convert to int value " << value << endl;
			exit(-1);
		}
	}
	float my_stof(string value) {
		float res = 0;
		try {
			res = stof(value);
			return res;
		}
		catch (...) {
			cout << "Impossible to convert to float value " << value << endl;
			exit(-1);
		}
	}
	string getValuesFromUser(Parser::Node* node) {
		string inputFromUser;
		if (node->kind == INPUT) {
			cout << node->op[0]->value << endl;
			//inputFromUser = "1";
			cin >> inputFromUser;
		}
		else if (node->kind == INT) {
			node = node->op[0];
			return getValuesFromUser(node);
		}
		return inputFromUser;
	}
	int parseExpr(Parser::Node* node) {
		string name;
		int result;
		for (int i = 0; i < node->op.size(); i++) {
			if (node->op[i]->kind == VAR) {
				if (node->op[i]->op.size() > 0 && node->op[i]->op[0]->kind == SET) {
					Kind type = parser.getType(node->op[i]->value);
					string value;
					if (node->op[i]->op[0]->op[0]->kind == INT || node->op[i]->op[0]->op[0]->kind == INPUT) {
						value = getValuesFromUser(node->op[i]->op[0]->op[0]);
					}
					else if (node->op[i]->op[0]->op[0]->kind == MAX) {
						result = max(valuesNumber[node->op[i]->op[0]->op[0]->op[0]->value], valuesNumber[node->op[i]->op[0]->op[0]->op[1]->value]);
						valuesNumber[node->op[i]->value] = result;
						continue;
					}
					
					else if(node->op[i]->op[0]->op[0]->kind == EXPR) {
						result = parseExpr(node->op[i]->op[0]->op[0]);
						valuesNumber[node->op[i]->value] = result;
						continue;
					}
					else{
						value = node->op[i]->op[0]->op[0]->value;
					}
					name = node->op[i]->value;
					float valueFloat;
					int valueInt;
					switch (type) {
					case CONSTS:
						valuesString.insert({ name, value });
						break;
					case CONSTN:
						if (valuesNumber.find(value) != valuesNumber.end()) {
							valuesNumber[name] = valuesNumber[value];
						}
						else if (valuesNumber.find(name) != valuesNumber.end()) {
							valueInt = my_stoi(value);
							valuesNumber[name] = valueInt;
						}
						else {
							valueInt = my_stoi(value);
							valuesNumber.insert({ name, valueInt });
						}
						break;
					case CONSTD:
						if (valuesFloat.find(value) != valuesFloat.end()) {
							valuesFloat[name] = valuesFloat[value];
						}
						else if (valuesNumber.find(name) != valuesNumber.end()) {
							valueFloat = my_stof(value);
							valuesFloat[name] = valueFloat;
						}
						else {
							valueFloat = my_stof(value);
							valuesFloat.insert({ name, valueFloat });
						}
						break;
					}
				}
				else if (node->op[i]->op.size() > 0 && std::find(operationsEq.begin(), operationsEq.end(), node->op[i]->op[0]->kind) != operationsEq.end()) {
					name = node->op[i]->value;
					switch (node->op[i]->op[0]->kind) {
					case PLUSEQ:
						if (valuesNumber.find(node->op[i]->op[0]->value) != valuesNumber.end()) {
							valuesNumber[name] += valuesNumber[node->op[i]->op[0]->value];
						}
						else if (valuesRange.find(node->op[i]->op[0]->value) != valuesRange.end()) {
							valuesNumber[name] += valuesRange[node->op[i]->op[0]->value][0];
						}
						else if (std::find(types.begin(), types.end(), node->op[i]->op[0]->op[0]->kind) != types.end()) {
							switch (node->op[i]->op[0]->op[0]->kind) {
							case CONSTN:
								valuesNumber[name] += my_stoi(node->op[i]->op[0]->op[0]->value);
								break;
							case CONSTD:
								valuesFloat[name] += my_stof(node->op[i]->op[0]->op[0]->value);
								break;
							case CONSTS:
								valuesString[name] += node->op[i]->op[0]->op[0]->value;
								break;
							}
						}
						else {
							cout << "Compiler error. Variable " << name << " isn't found" << endl;
							exit(-1);
						}
						break;
					case SUBEQ:
						if (valuesNumber.find(node->op[i]->op[0]->value) != valuesNumber.end()) {
							valuesNumber[name] -= valuesNumber[node->op[i]->op[0]->value];
						}
						else if (valuesRange.find(node->op[i]->op[0]->value) != valuesRange.end()) {
							valuesNumber[name] -= valuesRange[node->op[i]->op[0]->value][0];
						}
						else if (std::find(types.begin(), types.end(), node->op[i]->op[0]->op[0]->kind) != types.end()) {
							switch (node->op[i]->op[0]->op[0]->kind) {
							case CONSTN:
								valuesNumber[name] -= my_stoi(node->op[i]->op[0]->op[0]->value);
								break;
							case CONSTD:
								valuesFloat[name] -= my_stof(node->op[i]->op[0]->op[0]->value);
								break;
							}
						}
						else {
							cout << "Compiler error. Variable " << name << " isn't found" << endl;
							exit(-1);
						}
						break;
					case MULTEQ:
						if (valuesNumber.find(node->op[i]->op[0]->value) != valuesNumber.end()) {
							valuesNumber[name] *= valuesNumber[node->op[i]->op[0]->value];
						}
						else if (valuesRange.find(node->op[i]->op[0]->value) != valuesRange.end()) {
							valuesNumber[name] *= valuesRange[node->op[i]->op[0]->value][0];
						}
						else if (std::find(types.begin(), types.end(), node->op[i]->op[0]->op[0]->kind) != types.end()) {
							switch (node->op[i]->op[0]->op[0]->kind) {
							case CONSTN:
								valuesNumber[name] *= my_stoi(node->op[i]->op[0]->op[0]->value);
								break;
							case CONSTD:
								valuesFloat[name] *= my_stof(node->op[i]->op[0]->op[0]->value);
								break;
							}
						}
						else {
							cout << "Compiler error. Variable " << name << " isn't found" << endl;
							exit(-1);
						}
						break;
					case DIVEQ:
						if (valuesNumber.find(node->op[i]->op[0]->value) != valuesNumber.end()) {
							valuesNumber[name] /= valuesNumber[node->op[i]->op[0]->value];
						}
						else if (valuesRange.find(node->op[i]->op[0]->value) != valuesRange.end()) {
							valuesNumber[name] /= valuesRange[node->op[i]->op[0]->value][0];
						}
						else if (std::find(types.begin(), types.end(), node->op[i]->op[0]->op[0]->kind) != types.end()) {
							switch (node->op[i]->op[0]->op[0]->kind) {
							case CONSTN:
								valuesNumber[name] /= my_stoi(node->op[i]->op[0]->op[0]->value);
								break;
							case CONSTD:
								valuesFloat[name] /= my_stof(node->op[i]->op[0]->op[0]->value);
								break;
							}
						}
						else {
							cout << "Compiler error. Variable " << name << " isn't found" << endl;
							exit(-1);
						}
						break;
					}
				}
				else {
					name = node->op[i]->value;
				}
			}
			else if (std::find(operations.begin(), operations.end(), node->op[i]->kind) != operations.end()) {
			switch (node->op[i]->kind) {
			case ADD:
				if (valuesNumber.find(node->op[i]->op[0]->value) != valuesNumber.end()) {
					return valuesNumber[name] + valuesNumber[node->op[i]->op[0]->value];
				}
				else if (valuesRange.find(node->op[i]->op[0]->value) != valuesRange.end()) {
					return valuesNumber[name] + valuesRange[node->op[i]->op[0]->value][0];
				}
				else {
					cout << "Compiler error. Variable " << name << " isn't found" << endl;
					exit(-1);
				}
				break;
			case SUB:
				if (valuesNumber.find(node->op[i]->op[0]->value) != valuesNumber.end()) {
					return valuesNumber[name] - valuesNumber[node->op[i]->op[0]->value];
				}
				else if (valuesRange.find(node->op[i]->op[0]->value) != valuesRange.end()) {
					return valuesNumber[name] - valuesRange[node->op[i]->op[0]->value][0];
				}
				else {
					cout << "Compiler error. Variable " << name << " isn't found" << endl;
					exit(-1);
				}
				break;
			case MULTIPLY:
				if (valuesNumber.find(node->op[i]->op[0]->value) != valuesNumber.end()) {
					return valuesNumber[name] * valuesNumber[node->op[i]->op[0]->value];
				}
				else if (valuesRange.find(node->op[i]->op[0]->value) != valuesRange.end()) {
					return valuesNumber[name] * valuesRange[node->op[i]->op[0]->value][0];
				}
				else {
					cout << "Compiler error. Variable " << name << " isn't found" << endl;
					exit(-1);
				}
				break;
			case DIVIDE:
				if (valuesNumber.find(node->op[i]->op[0]->value) != valuesNumber.end()) {
					return valuesNumber[name] / valuesNumber[node->op[i]->op[0]->value];
				}
				else if (valuesRange.find(node->op[i]->op[0]->value) != valuesRange.end()) {
					return valuesNumber[name] / valuesRange[node->op[i]->op[0]->value][0];
				}
				else {
					cout << "Compiler error. Variable " << name << " isn't found" << endl;
					exit(-1);
				}
				break;
			}
			}
			else if (node->op[i]->kind == PRINT) {
				parsePrint(node->op[i]);
			}
		}
		return 0;
	}
	void parsePrint(Parser::Node* node) {
		if (valuesNumber.find(node->op[0]->value) != valuesNumber.end()) {
			cout << valuesNumber[node->op[0]->value] << endl;
		}else if (valuesFloat.find(node->op[0]->value) != valuesFloat.end()) {
			cout << valuesFloat[node->op[0]->value] << endl;
		}else if (valuesString.find(node->op[0]->value) != valuesString.end()) {
			cout << valuesString[node->op[0]->value] << endl;
		}
		else {
			cout << node->op[0]->value << endl;
		}
	
	}

	bool parseCheckExpr(Parser::Node* node) {
		string name = node->value;

		float valueFloat;
		int valueInt;
		string valueStr;

		if (std::find(rules.begin(), rules.end(), node->op[0]->kind) != rules.end()) {
			switch (node->op[0]->kind) {
			case LT:
				valueStr = node->op[0]->op[0]->value;
				if (node->op[0]->op[0]->kind == VAR) {
					switch (parser.getType(valueStr)) {
					case CONSTN:
						return (valuesNumber[name] < valuesNumber[valueStr]);
					case CONSTD:
						return(valuesFloat[name] < valuesFloat[valueStr]);
					case CONSTS:
						return(valuesString[name] < valuesString[valueStr]);
					}
				}
				switch (node->op[0]->op[0]->kind) {
				case CONSTN:
					valueInt = my_stoi(valueStr);
					return (valuesNumber[name] < valueInt);
				case CONSTD:
					valueFloat = my_stof(valueStr);
					return (valuesFloat[name] < valueFloat);
				case CONSTS:
					return (valuesString[name] < valueStr);
				}				
				break;
			case LE:
				valueStr = node->op[0]->op[0]->value;
				if (node->op[0]->op[0]->kind == VAR) {
					switch (parser.getType(valueStr)) {
					case CONSTN:
						return (valuesNumber[name] <= valuesNumber[valueStr]);
					case CONSTD:
						return(valuesFloat[name] <= valuesFloat[valueStr]);
					case CONSTS:
						return(valuesString[name] <= valuesString[valueStr]);
					}
				}
				switch (node->op[0]->op[0]->kind) {
				case CONSTN:
					valueInt = my_stoi(valueStr);
					return (valuesNumber[name] <= valueInt);
				case CONSTD:
					valueFloat = my_stof(valueStr);
					return (valuesFloat[name] <= valueFloat);
				case CONSTS:
					return (valuesString[name] <= valueStr);
				}
				break;
			case EQ:
				valueStr = node->op[0]->op[0]->value;
				if (node->op[0]->op[0]->kind == VAR) {
					switch (parser.getType(valueStr)) {
					case CONSTN:
						return (valuesNumber[name] == valuesNumber[valueStr]);
					case CONSTD:
						return(valuesFloat[name] == valuesFloat[valueStr]);
					case CONSTS:
						return(valuesString[name] == valuesString[valueStr]);
					}					
				}
				switch (node->op[0]->op[0]->kind) {
				case CONSTN:
					valueInt = my_stoi(valueStr);
					return (valuesNumber[name] == valueInt);
				case CONSTD:
					valueFloat = my_stof(valueStr);
					return (valuesFloat[name] == valueFloat);
				case CONSTS:
					return (valuesString[name] == valueStr);
				}
				break;
			case NEQ:
				valueStr = node->op[0]->op[0]->value;
				switch (node->op[0]->op[0]->kind) {
				case CONSTN:
					valueInt = my_stoi(valueStr);
					return (valuesNumber[name] != valueInt);
				case CONSTD:
					valueFloat = my_stof(valueStr);
					return (valuesFloat[name] != valueFloat);
				case CONSTS:
					return (valuesString[name] != valueStr);
				}
				break;
			case GT:
				valueStr = node->op[0]->op[0]->value;
				if (node->op[0]->op[0]->kind == VAR) {
					switch (parser.getType(valueStr)) {
					case CONSTN:
						return (valuesNumber[name] > valuesNumber[valueStr]);
					case CONSTD:
						return(valuesFloat[name] > valuesFloat[valueStr]);
					case CONSTS:
						return(valuesString[name] > valuesString[valueStr]);
					}
				}
				switch (node->op[0]->op[0]->kind) {
				case CONSTN:
					valueInt = my_stoi(valueStr);
					return (valuesNumber[name] > valueInt);
				case CONSTD:
					valueFloat = my_stof(valueStr);
					return (valuesFloat[name] > valueFloat);
				case CONSTS:
					return (valuesString[name] > valueStr);
				}
				break;
			case GE:
				valueStr = node->op[0]->op[0]->value;
				if (node->op[0]->op[0]->kind == VAR) {
					switch (parser.getType(valueStr)) {
					case CONSTN:
						return (valuesNumber[name] >= valuesNumber[valueStr]);
					case CONSTD:
						return(valuesFloat[name] >= valuesFloat[valueStr]);
					case CONSTS:
						return(valuesString[name] >= valuesString[valueStr]);
					}
				}
				switch (node->op[0]->op[0]->kind) {
				case CONSTN:
					valueInt = my_stoi(valueStr);
					return (valuesNumber[name] >= valueInt);
				case CONSTD:
					valueFloat = my_stof(valueStr);
					return (valuesFloat[name] >= valueFloat);
				case CONSTS:
					return (valuesString[name] >= valueStr);
				}
				break;
			}
		}
		return false;
	}

	bool parseIf(Parser::Node* node) {
		bool res = parseCheckExpr(node->op[0]);
		if (res) {
			parseExpr(node->op[1]);
		}
		return res;
	}
	void parseFor(Parser::Node* node) {
		valuesRange.insert({ node->op[0]->value, {0, 0} });
		int index;
		if (node->op[1]->op.size() == 1) {
			valuesRange[node->op[0]->value][0] = 0;
			index = 0;
		}
		else if(node->op[1]->op.size() > 1) {
			valuesRange[node->op[0]->value][0] = my_stoi(node->op[1]->op[0]->value);
			index = 1;
		}
		else {
			cout << "Range must have at least 1 parametr" << endl;
			exit(-1);
		}
		for (int j = index; j < node->op[1]->op.size(); j++) {
			for (int k = 0; k < node->op[1]->op[j]->op.size(); k++) {
				switch (node->op[1]->op[j]->op[k]->kind) {
				case VAR:
					valuesRange[node->op[0]->value][1] = valuesNumber[node->op[1]->op[j]->op[k]->value];
					break;
				case ADD:
					valuesRange[node->op[0]->value][1] += my_stoi(node->op[1]->op[j]->op[k]->op[0]->value);
					break;
				case SUB:
					valuesRange[node->op[0]->value][1] -= my_stoi(node->op[1]->op[j]->op[k]->op[0]->value);
					break;
				case MULTIPLY:
					valuesRange[node->op[0]->value][1] *= my_stoi(node->op[1]->op[j]->op[k]->op[0]->value);
					break;
				case DIVIDE:
					valuesRange[node->op[0]->value][1] /= my_stoi(node->op[1]->op[j]->op[k]->op[0]->value);
					break;
				}
			}
		}
		int start = valuesRange[node->op[0]->value][0];
		bool isIf = false;
		for (int i = start; i < valuesRange[node->op[0]->value][1]; i++) {
			for (int j = 2; j < node->op.size(); j++) {
				if (isIf && (node->kind == ELSE || node->kind == ELIF)) {
					continue;
				}
				else {
					isIf = parseNode(node->op[j]);
				}
			}
			valuesRange[node->op[0]->value][0] += 1;
		}
	}
	
	void parseWhile(Parser::Node* node) {
		bool isIf=false;
		while (parseCheckExpr(node->op[0])) {
			for (int i = 1; i < node->op.size(); i++) {
				if (isIf && (node->op[i]->kind == ELSE || node->op[i]->kind==ELIF)) {
					continue;
				}
				else {
					isIf = parseNode(node->op[i]);
				}
			}
		}
	}
	bool parseNode(Parser::Node* node) {
		bool isIf = false;
		switch (node->kind) {
			case EXPR:
				parseExpr(node);
				break;
			case IF:
			case ELIF:
				if (!isIf) {
					isIf = parseIf(node);
				}
				break;
			case ELSE:
				if (!isIf) {
					parseProg(node);
				}
				break;
			case FOR:
				parseFor(node);
				break;
			case WHILE:
				parseWhile(node);
				break;
			}
		return isIf;
	}
	bool parseProg(Parser::Node* root) {
		bool isIf = false;
		for (int i = 0; i < root->op.size(); i++) {	
			switch (root->op[i]->kind) {
			case EXPR:
				parseExpr(root->op[i]);
				break;
			case IF:
			case ELIF:
				if (!isIf) {
					isIf = parseIf(root->op[i]);
				}
				break;
			case ELSE:
				if (!isIf) {
					parseProg(root->op[i]);
				}
				break;
			case FOR:
				parseFor(root->op[i]);
				break;
			case WHILE:
				parseWhile(root->op[i]);
				break;
			}
		}
		return isIf;
	}

	void compile() {
		Parser::Node* root = parser.Parse();
		parseProg(root);
	}
};