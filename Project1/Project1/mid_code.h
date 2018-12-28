#pragma once
#include <iostream>
#include <vector>
#include "difi.h"
#include "table.h"
using namespace std;
extern int state_count;
extern bool write_file;
string create_tmp();
string kind2string(Kind kind);
void var_declar_mid(VarTableEntry* variable);
void func_declar_mid(FuncTableEntry* function);
void para_declar_mid(Type type, string name);
void push_mid(string name);
void push_mid(int name);
void print_mid(Type tp, string s);
void print_mid(Type tp, int s);
void scanf_mid(Type tp, string s);
void scanf_mid(Type tp, int s);
void return_mid(FuncTableEntry* function);
void return_mid(int i);
void return_mid(string s);
string creat_label(FuncTableEntry* function, string s);
void cond_mid(Symbol op,string result, string left, string right);
void cond_mid(Symbol op, string result, int left, string right);
void cond_mid(Symbol op, string result, string left, int right);
void assign_mid(string s1, string s2);
void assign_mid(string s1, int a);
void array_get_mid(string result, string array_name, string offset);
void array_get_mid(string result, string array_name, int offset);
void func_call_mid(string name);
void return_get_mid(string name);
void branch_without_mid(string label);
void branch_meet_mid(string name, string label);
void branch_not_mid(string name, string label);
void label_mid(string label);
void branch_equal_mid(string name, int value, string label);
void array_set_mid(string array_name, string offset, int value);
void array_set_mid(string array_name, int offset, int value);
void array_set_mid(string array_name, int offset, string value);
void array_set_mid(string array_name, string offset, string value);
void exit_mid();
void same_mid(string code);