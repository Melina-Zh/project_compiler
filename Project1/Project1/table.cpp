#include <iostream>
#include <string>
#include "table.h"
#include "difi.h"
#include "syntax.h"
#include "mid_code.h"
#include "lexer.h"

using namespace std;
string kind2string(Kind kind)
{
	switch (kind)
	{
	case FUNC:
		return "function";
	case CONST:
		return "constant";
	case VAR:
		return "variable";
	}
	return NULL;
}
string type2string(Type type)
{
	switch (type)
	{
	case INT:
		return "int";
	case CHAR:
		return "char";
	case VOID:
		return "void";
	case STRING:
		return "string";
	}
	return NULL;
}
TableEntry::TableEntry(string na, Type ty, Kind ki) :
	name(na), type(ty), kind(ki)
{}

VarTableEntry::VarTableEntry(string na, Type ty, int l) :
	TableEntry(na, ty, VAR), len(l)
{}
bool VarTableEntry::isarray() {
	return len != 0;
}
int VarTableEntry::get_len() {
	return len;
}
int ConstTableEntry::get_value() {
	return value;
}
FuncTableEntry::FuncTableEntry(string na, Type ty) : TableEntry(na, ty, FUNC)
{
	size = 0;
}
bool FuncTableEntry::has_var(string name) {
	map<string, VarTableEntry*>::iterator each = local_vartable.find(name);
	return (each != local_vartable.end());
}
bool  FuncTableEntry::has_const(string name) {
	map<string, ConstTableEntry*>::iterator each = local_consttable.find(name);
	return (each != local_consttable.end());
}
void FuncTableEntry::add_const(string name, Type type, int value) {
	if (has_var(name) || has_const(name)) {
		error("redefinition of '" + name + "'");
		return;
	}
	else if (get_func(name) != NULL) {
		error("There is a function with the same name!");
		return;
	}
	ConstTableEntry* c = new ConstTableEntry(name, type, value);
	local_consttable.insert(map<string, ConstTableEntry*>::value_type(name, c));
}
void FuncTableEntry::add_var(string name, Type type, int len) {
	if (has_var(name) || has_const(name)) {
		error("redefinition of '" + name + "'");
		return;
	}
	else if (get_func(name) != NULL) {
		error("There is a function with the same name!");
		return;
	}
	VarTableEntry* var_e = new VarTableEntry(name, type, len);
	local_vartable.insert(map<string, VarTableEntry*>::value_type(name, var_e));
	var_declar_mid(var_e);
}
void FuncTableEntry::add_para(string name, Type type) {
	if (has_var(name)) {
		error("redefinition of '" + name + "'");
		return;
	}
	else if (get_func(name) != NULL) {
		error("There is a function with the same name!");
		return;
	}
	VarTableEntry* var_e = new VarTableEntry(name, type);
	local_vartable.insert(map<string,VarTableEntry*>::value_type(name, var_e));
	paralist.push_back(var_e);
	para_declar_mid(type, name);
}
ConstTableEntry* FuncTableEntry::search_cons(string name) {
	map<string, ConstTableEntry*>::iterator each = local_consttable.find(name);
	if (each == local_consttable.end()) {
		return NULL;
	}
	return each->second;
}
VarTableEntry* FuncTableEntry::search_var(string name) {
	map<string, VarTableEntry*>::iterator each = local_vartable.find(name);
	if (each == local_vartable.end()) {
		return NULL;
	}
	return each->second;
}
bool FuncTableEntry::check_para(vector<Type> t) {
	if (t.size() != paralist.size()) {
		error("The number of parameters does not match");
		return  false;
	}
	for (int i = 0; i <(t.size()); i++) {
		if (t[i] != paralist[i]->get_type()) {
			return false;
		}
	}
	return true;
}
int FuncTableEntry::get_para_count(){

	return paralist.size();
}