#pragma once
#include <iostream>
#include <map>
#include<vector>
#include "difi.h"

using namespace std;
//construct TableEntry¡ª¡ªobject-oriented
string type2string(Type type);
string kind2string(Type type);
class TableEntry {
private :
	string name;
	Type type;
	Kind kind;
public:
	TableEntry(string na, Type ty, Kind ki);
	//to print message
	string get_name() {
		return name;
	}
	Type get_type() {
		return type;
	}
	Kind get_kind() {
		return kind;
	}
};
//inherited from TableEntry
class VarTableEntry:public TableEntry {
private:
	int len;
public:
	VarTableEntry(string na, Type ty, int l = 0);
	bool isarray();
	int get_len();

};

class ConstTableEntry :public TableEntry {
private:
	int value;
public:
	ConstTableEntry(string na, Type ty, int va) :
		TableEntry(na, ty, CONST),value(va)
	{}
	int get_value();
};

class FuncTableEntry :public TableEntry {
private:
	vector<VarTableEntry*> paralist;
	map<string, ConstTableEntry*> local_consttable;
	map<string, VarTableEntry*> local_vartable;
	int size;
	
public:
	FuncTableEntry(string na, Type ty);
	bool has_var(string name);
	bool has_const(string name);
	void add_const(string name, Type type, int value);
	void add_var(string name, Type type, int len=0);
	void add_para(string name, Type type);
	ConstTableEntry* search_cons(string name);
	VarTableEntry* search_var(string name);
	bool check_para(vector<Type> t);
	int get_para_count();
	
};
