#pragma once
#ifndef DIFI_H_INCLUDED
#define DIFI_H_INCLUDED
#define MAX_TOKEN_LENTH 1000
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;
typedef enum {
	IDENT, INTCON, CHARCON, STRCON, ZERO,

	SQUO, DQUO, COMMA, COLON, SEMI, LPAR, RPAR,
	LBKT, RBKT, LBRACE, RBRACE, ASS, ADD, SUB, MUL,
	DIV, EQ, NE, GT, GE, LT, LE,

	CHARSY, INTSY, VOIDSY, CONSTSY, IFSY, ELSY, WHILESY,
	SWTSY, CASESY, DFTSY, SCFSY, PRTFST, RTNSY, MAINSY
} Symbol;

typedef enum {
	VOID, CHAR, INT, STRING
}Type;
typedef enum {
	CONST, VAR, FUNC
}Kind;
extern char current_char;
extern FILE *File;
extern ofstream fout;
extern ofstream f_mid;
extern int file_no;
extern string mid_filename;
extern vector<string> strs;
extern char token[MAX_TOKEN_LENTH];
extern int token_top;
extern int number;
extern Symbol symbol;

#endif // DIFI_H_INCLUDED
