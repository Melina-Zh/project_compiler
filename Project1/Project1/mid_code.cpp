#include <iostream>
#include <string>
#include "mid_code.h"
#include "table.h"
#include "difi.h"
#include "lexer.h"
#include <cstdlib>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;
bool write_file;
vector<string> strs;
int tmp_id = 0;
string create_tmp() {
	stringstream tmpo;
	tmpo << "@t" << tmp_id++;
	return tmpo.str();
}
//#define enumtoStr(val) Type.val.ToString()
/*function declaration code*/
void func_declar_mid(FuncTableEntry* function) {
	
	fout << type2string(function->get_type())<<" "<<function->get_name()<<" ()" <<endl;
}
/*parameter with a function*/
void para_declar_mid(Type type,string name) {
	
	fout << "#para " << type2string(type) << " "<<name << endl;
}
/*invoke a function, only the 'call'code*/
void func_call_mid(string name) {
	
	fout <<"#call "<< name << endl;
}
/*assign code*/
void assign_mid(string s1,string s2) {
	
	fout << s1 << " = " << s2<<endl;
} 
void assign_mid(string s1,int a) {
	
	fout << s1 << " = " << a<<endl;
}
/*push code*/
void push_mid(string name) {
	
	fout <<"#push "<< name << endl;
}
void push_mid(int name) {
	
	fout << "#push " << name << endl;
}

/*return statement code*/
void return_mid(FuncTableEntry* function) {
	if (function->get_type() == VOID) {
		
		fout << "#ret " << endl;
	}
	else {
			
		fout << "#ret 0" << endl;
	}
}
void return_mid(int i) {
	
	fout << "#ret " << i<<endl;
}
void return_mid(string s) {
	
	fout << "#ret " << s << endl;
}
/*condition code*/
void cond_mid(Symbol op,string result, string left,string right) {
	
	fout <<result<<" = "<< left << " " << symbol2string(op) << " " << right<<endl;
}
void cond_mid(Symbol op, string result, string left, int right) {
	
	fout << result << " = " << left <<" "<< symbol2string(op) << " " << right << endl;
}
void cond_mid(Symbol op, string result, int left, string right) {
	
	fout << result << " = " << left << " " << symbol2string(op) << " " << right << endl;
}

/*label code*/
void label_mid(string label) {
	
	fout << label << " :"<<endl;
}

/*variable declaration code*/
void var_declar_mid(VarTableEntry* variable) {
	if (variable->isarray())
	{
		
		fout << "#array " << type2string(variable->get_type()) << " " << variable->get_name() <<" [] "<< variable->get_len()<<endl;
	}
	else
	{
		
		fout<<"#var " << type2string(variable->get_type()) << " " << variable->get_name()<<endl;
	}
}
/*branch without condition GOTO*/
void branch_without_mid(string label) {
	
	fout << "#GOTO " << label<<endl;
}
/*meet these conditions, branch BNZ  not zero*/ 
void branch_meet_mid(string name,string label) {
	
	fout << "#BNZ " << name << " " << label << endl;
}
/*not, branch BZ*/
void branch_not_mid(string name,string label) {

	fout << "#BZ " << name << " " << label << endl;
}
/*equal, branch BE*/
void branch_equal_mid(string name,int value, string label) {

	fout << "#BE " << name << " "<<value<<" " << label << endl;
}
/*get array code*/
void array_get_mid(string result,string array_name,string offset) {
	
	fout << result << " = " << array_name << " [ " << offset << " ] "<<endl;
}
void array_get_mid(string result, string array_name, int offset) {

	fout << result << " = " << array_name << " [ " << offset << " ] "<<endl;
}
/*set array code*/
void array_set_mid(string array_name,string offset,int value){
	
	fout << array_name << " [ " << offset << " ] " << " = " << value << endl;
}
void array_set_mid(string array_name, int offset, int value) {

	fout << array_name << " [ " << offset << " ] " << " = " << value << endl;
}
void array_set_mid(string array_name, int offset, string value) {

	fout << array_name << " [ " << offset << " ] " << " = " << value << endl;
}
void array_set_mid(string array_name, string offset, string value) {
	
	fout << array_name << " [ " << offset << " ] " << " = " << value << endl;
}
/*print code*/
void print_mid(Type tp,string s) {
	if (tp == STRING) {
		
		for (int i = 0; i < strs.size(); i++)
			{
				if (strs[i] == s)//the string exist
				{
					
					fout << "#printf " << type2string(tp) <<  " string" << i << endl;
			
					return;
				}
			}
				
		fout << "#printf " << type2string(tp) << " string" << strs.size() << endl;
		strs.push_back(s);
		
	}
	
	else
	{
					
		fout<<"#printf " << type2string(tp) << " " << s<<endl;
	}
}
void print_mid(Type tp, int s) {
	
	fout << "#printf " << type2string(tp) << " " << s << endl;
}
/*scanf code*/
void scanf_mid(Type tp, string s) {
	
	fout << "#scanf " << type2string(tp) << " " << s << endl;
}
void scanf_mid(Type tp, int s) {
	
	fout << "#scanf " << type2string(tp) << " " << s << endl;
}
/*code without any change(souce code is the same as quadruple)*/
void same_mid(string code) {
	
	fout << code << endl;
}
/*creat a new label*/
string creat_label(FuncTableEntry* function,string s) {
	stringstream label_tmp;
	label_tmp <<function->get_name()<< "_" <<s<<state_count++;
	return label_tmp.str();
}
void return_get_mid(string name) {
	
	fout << "#get " << name<<endl;
}
void exit_mid() {
	
	fout << "#exit" << endl;
}