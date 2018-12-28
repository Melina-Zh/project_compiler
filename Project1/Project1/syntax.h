#pragma once
#include<iostream>
#include<fstream>
#include<map>
#include<vector>
using namespace std;
class TableEntry;
class FuncTableEntry;
class VarTableEntry;
class ConstTableEntry;
void syn_main();
FuncTableEntry* get_func(string name);
VarTableEntry* search_global_var(string name);

extern map<string, VarTableEntry*> vartable;
extern map<string, FuncTableEntry*> functable;
extern map<string, ConstTableEntry*> consttable;