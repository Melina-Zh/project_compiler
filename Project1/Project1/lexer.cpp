#include<iostream>
#include<string>
using namespace std;
#include "difi.h"
#include "lexer.h"
char current_char;
char token[MAX_TOKEN_LENTH];
int token_top = 0;
int number = 0;
Symbol symbol;
int line_no = 1;
void error(string info) {
	cout << "[ERROR] " << info <<" at [LINE] "<<line_no<< endl;
}


string symbol2string(Symbol sym) {
	switch (sym) {
	case IDENT:
		return "IDENT";
	case INTCON:
		return "INTCON";
	case CHARCON:
		return "CHARCON";
	case STRCON:
		return "STRCON";
	case ZERO:
		return "ZERO";
	case SQUO:
		return "SQUO";
	case DQUO:
		return "DQUO";
	case COMMA:
		return "COMMA";
	case COLON:
		return "COLON";
	case SEMI:
		return "SEMI";
	case LPAR:
		return "LPAR";
	case RPAR:
		return "RPAR";
	case LBKT:
		return "LBKT";
	case RBKT:
		return "RBKT";
	case LBRACE:
		return "LBRACE";
	case RBRACE:
		return "RBRACE";
	case ASS:
		return "ASS";
	case ADD:
		return "ADD";
	case SUB:
		return "SUB";
	case MUL:
		return "MUL";
	case DIV:
		return "DIV";
	case EQ:
		return "EQ";
	case NE:
		return "NE";
	case GT:
		return "GT";
	case GE:
		return "GE";
	case LT:
		return "LT";
	case LE:
		return "LE";
	case INTSY:
		return "INTSY";
	case CHARSY:
		return "CHARSY";
	case VOIDSY:
		return "VOIDSY";
	case CONSTSY:
		return "CONSTSY";
	case IFSY:
		return "IFSY";
	case WHILESY:
		return "WHILESY";
	case SWTSY:
		return "SWTSY";
	case CASESY:
		return "CASESY";
	case DFTSY:
		return "DFTSY";
	case SCFSY:
		return "SCFSY";
	case PRTFST:
		return "PRTFST";
	case RTNSY:
		return "RTNSY";
	case MAINSY:
		return "MAINSY";
	default:
		error("wrong symbol");
		return "";
	}
}
int num2int() {//unsigned
	int num = 0;
	for (int i = 0; i < token_top; i++) {
		num *= 10;
		num += token[i] - '0';
	}
	return num;
}

void print_sym(Symbol sym, char value[]) {
	/*
	string sym_str = symbol2string(sym);
	cout << sym_str << ' ' << value << endl;*/
}
void print_sym(Symbol sym, char value) {
	/*
	string sym_str = symbol2string(sym);
	cout << sym_str << ' ' << value << endl;*/
}
void print_sym(Symbol sym, int value) {
	/*
	string sym_str = symbol2string(sym);
	cout << sym_str << ' ' << value << endl;*/
}

void clearToken() {
	token_top = 0;
}
bool isSpace() {
	return (current_char == ' ');
}
bool isEnter() {
	return (current_char == '\n');
}
bool isTab() {
	return (current_char == '\t');
}
bool isBlank() {
	return (isSpace() || isEnter() || isTab());
}
bool isLetter() {
	return ((current_char == '_') || (current_char >= 'A' && current_char <= 'Z')
		|| (current_char >= 'a' && current_char <= 'z'));
}
bool isDigit() {
	return (current_char >= '0'&&current_char <= '9');
}
bool isColon() {
	return (current_char == ':');
}

bool isSquo() {
	return (current_char == '\'');
}

bool isDquo() {
	return (current_char == '\"');
}

bool isComma() {
	return (current_char == ',');
}

bool isSemi() {
	return (current_char == ';');
}

bool isLpar() {
	return (current_char == '(');
}

bool isRpar() {
	return (current_char == ')');
}

bool isLbkt() {
	return (current_char == '[');
}

bool isRbkt() {
	return (current_char == ']');
}

bool isLbrace() {
	return (current_char == '{');
}

bool isRbrace() {
	return (current_char == '}');
}

bool isEqu() {
	return (current_char == '=');
}

bool isAdd() {
	return (current_char == '+');
}

bool isSub() {
	return (current_char == '-');
}

bool isMul() {
	return (current_char == '*');
}

bool isDiv() {
	return (current_char == '/');
}

bool isGT() {
	return (current_char == '>');
}

bool isLT() {
	return (current_char == '<');
}

bool isExcal() {
	return (current_char == '!');
}

bool isStringChar() {
	return (current_char >= 32 && current_char <= 126 && current_char != 34);
}

bool read_char_main() {
	current_char = fgetc(File);
	if (current_char == EOF) {
		return true;
	}
	else return false;
}
bool read_char() {
	if (current_char == EOF) {
			error("missing some characters");
			return false;
		}
	current_char = fgetc(File);
	if (current_char == EOF) {
		error("missing some characters");
		return false;
	}
	if (current_char == '\n') {
		line_no++;
	}
	
	return (current_char != EOF);
}

void retract() {
	if (current_char != 0) {
		if (current_char == '\n')
			line_no--;
			ungetc(current_char, File);
			current_char = 0;
			return;
	}
	
}

//get the symbol
Symbol get_symbol()
{
	token[token_top] = 0;
	if (strcmp(token, "char") == 0)
	{
		return CHARSY;
	}
	else if (strcmp(token, "int") == 0)
	{
		return INTSY;
	}
	else if (strcmp(token, "void") == 0)
	{
		return VOIDSY;
	}
	else if (strcmp(token, "const") == 0)
	{
		return CONSTSY;
	}
	else if (strcmp(token, "if") == 0)
	{
		return IFSY;
	}
	else if (strcmp(token, "while") == 0)
	{
		return WHILESY;
	}
	else if (strcmp(token, "switch") == 0)
	{
		return SWTSY;
	}
	else if (strcmp(token, "case") == 0)
	{
		return CASESY;
	}
	else if (strcmp(token, "default") == 0)
	{
		return DFTSY;
	}
	else if (strcmp(token, "scanf") == 0)
	{
		return SCFSY;
	}
	else if (strcmp(token, "printf") == 0)
	{
		return PRTFST;
	}
	else if (strcmp(token, "return") == 0)
	{
		return RTNSY;
	}
	else if (strcmp(token, "main") == 0)
	{
		return MAINSY;
	}
	else
	{
		return IDENT;
	}
}
bool end()//if the next character is EOF
{

	do {
		read_char();
		
	} while (isSpace() || isEnter() || isTab());
	
	if (current_char != EOF)
	{
	
			retract();
		
		return false;
	}
	return true;
}
bool getsym() {
	clearToken();
	if (!read_char())
		return false;
	while (isBlank()) {//skip all space
		if (!read_char())
			return false;
	}
	if (isLetter()) {//read a letter
		while (isLetter() || isDigit()) {
			
			token[token_top++] = current_char;
			if (!read_char())
				return false;
		}
		token[token_top] = 0;
		retract();
		symbol = get_symbol();
		print_sym(symbol, token);

	}
	else if (isDigit()) {
		bool zero = false;
		if (current_char == '0') {
			zero = true;
		}
		while (isDigit()) {

			token[token_top++] = current_char;
			if (!read_char()) return false;
			
		}
		if (token_top > 1) {
			if (token[0] == '0'&&token[1] == '0') {
				error("unexpected leading zero");
			}
		}
		
		retract();
		number = num2int();
		if (zero && number != 0) {
			symbol = INTCON;//no leading zero
			error("unexpected leading zero");
		}
		else if (zero) {
			symbol = ZERO;
		}
		else {
			symbol = INTCON;
		}
		print_sym(symbol, number);

	}
	else if (isColon()) {//':'
		symbol = COLON;
		print_sym(symbol, current_char);
	}
	else if (isComma()) {//','
		symbol = COMMA;
		print_sym(symbol, current_char);
	}
	else if (isSemi()) {//';'
		symbol = SEMI;
		print_sym(symbol, current_char);
	}
	else if (isLpar()) {//'('
		symbol = LPAR;
		print_sym(symbol, current_char);
	}
	else if (isRpar()) {//')'
		symbol = RPAR;
		print_sym(symbol, current_char);
	}
	else if (isLbkt()) {//'['
		symbol = LBKT;
		print_sym(symbol, current_char);
	}
	else if (isRbkt()) {//']'
		symbol = RBKT;
		print_sym(symbol, current_char);
	}
	else if (isLbrace()) {//'{'
		symbol = LBRACE;
		print_sym(symbol, current_char);
	}
	else if (isRbrace()) {//'}'
		symbol = RBRACE;
		print_sym(symbol, current_char);
	}
	else if (isAdd()) {
		symbol = ADD;
		print_sym(symbol, current_char);
	}
	else if (isSub()) {
		symbol = SUB;
		print_sym(symbol, current_char);
	}
	else if (isMul()) {
		symbol = MUL;
		print_sym(symbol, current_char);
	}
	else if (isDiv()) {
		symbol = DIV;
		print_sym(symbol, current_char);
	}
	else if (isSquo()) {//single quotes

		symbol = CHARCON;
		number = -1;
		if (!read_char())
			return false;
		if (isAdd() || isSub() || isMul() || isDiv() || isLetter() || isDigit()) {
			number = (int)current_char;
		}
		else if (isSquo()) {
			print_sym(symbol, number);
			error("no character");
			return true;	// is ''
		}
		else {
			error("invalid character");
		}

		// the other '
		if (!read_char()) return false;
		if (!isSquo()) {
			number = -1;
			error("it is supposed to be a character, not a string");

			while (!isSquo()) {
				if (!read_char()) return false;
			}

		}
		print_sym(symbol, number);

	}
	else if (isDquo()) {
		symbol = STRCON;

		// mate string
		while (true) {
			if (!read_char()) return false;
			if (isDquo()) {// double quotes
				token[token_top] = 0; // set tailed
				print_sym(symbol, token);
				break;
			}
			else if (!isStringChar()) {
				error("invalid char in string");
				continue;
			}
			else {
				if (current_char == '\\') {
					token[token_top++] = '\\';
				}
				token[token_top++] = current_char;
			}
			if (current_char == EOF) {
				error("It is supposed to have a double quote");
				exit(0);
			}


		}

	}
	else if (isExcal()) { // !
		token[token_top++] = current_char;
		if (!read_char()) return false;
		if (isEqu()) { // =
			token[token_top++] = current_char;
			symbol = NE;
			token[token_top] = 0;
			print_sym(symbol, token);
		}
		else {
			retract();
			error("wrong token '!'");
		}

	}
	else if (isEqu()) {
		if (!read_char()) return false;
		if (isEqu()) {
			symbol = EQ;
			print_sym(symbol, token);
		}
		else {
			retract();
			symbol = ASS;
			print_sym(symbol, current_char);
		}

	}
	else if (isGT()) {     // >
		token[token_top++] = current_char;
		if (!read_char()) return false;
		if (isEqu()) {// =
			token[token_top++] = current_char;
			symbol = GE;
		}
		else {
			retract();
			symbol = GT;
		}
		token[token_top] = 0;
		print_sym(symbol, token);

	}
	else if (isLT()) { // <
		token[token_top++] = current_char;
		if (!read_char()) return false;
		if (isEqu()) {// =
			token[token_top++] = current_char;
			symbol = LE;
		}
		else {
			retract();
			symbol = LT;
		}
		token[token_top] = 0;
		print_sym(symbol, token);

	}

	else {
		error((string)"Unknown token !");
		return getsym();    // skip
	}
	return true;
}
