#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <vector>
#include "difi.h" 
#include "table.h"
#include "syntax.h"
#include "lexer.h"
#include "mips.h"
/*一个整数4个字节 一个字符1个字节*/
//ofstream fout;
using namespace std;
ifstream fin;
FuncTableEntry* func_c = NULL;
vector<string> free_temps;
const int reg_count = 8;
string mid_filename;
int next_reg = 0;
map<string, int>reg_map;
map<string, int>global_addr_map;
map<string, int>offset_map;
string label_e;
Reg reg_regis[20];
int temp_base_addr = 0;     
int int_position = 0;
int char_position = 0;
int cur_addr = 0;
int para_read_count = 0;
vector<string> para_vector;
int get_reg(string name, vector<string>* conf_names);
void array_mips(string arr_str, string off_str, string sou_str, bool is_set);
int round_up(int num, int unit)//对齐
{
	if (num % unit == 0)
	{
		return num;
	}
	else
	{
		num /= unit;
		num++;
		num *= unit;
		return num;
	}
}
bool is_temp(string name) {
	return name[0] == '@';
}

int get_temp_no(string name) {
	name.erase(0, 2);
	int i;
	sscanf(name.c_str(), "%d", &i);
	return i;
}
bool is_num(string str)
{
	int len = str.size();
	int i;
	if (str.at(0) == '-')
	{
		i = 1;
	}
	else
	{
		i = 0;
	}
	for (; i < len; i++)
	{
		if (str.at(i) < '0' || str.at(i) > '9')
		{
			return false;
		}
	}
	return true;
}


void init_reg_map() {
	reg_map.clear();
	for (int i = 0; i < reg_count; i++) {
		Reg* regi = &reg_regis[i];
		if ((regi->reg_state)) {
			if (regi->type == INT) {
				if (regi->global) {
					fout<<"sw $s" << i << ", " << regi->offset << "($gp)"<<endl;//store the global value 
				}
				else {
					fout<<"sw $s" << i << ", " << regi->offset << "($fp)" << endl;
				}

			}
			else {    // CHAR
				if (regi->global) {
					fout<<"sb $s" << i << ", " << regi->offset << "($gp)" << endl;
				}
				else {
					fout<<"sb $s" << i << ", " << regi->offset << "($fp)" << endl;
				}

			}
			regi->reg_state=false;//store and convert state
		}
	}
}

void gen_var(VarTableEntry* variable) {
	if (variable->isarray()) {
		if (variable->get_type() == CHAR) {
			if (int_position > char_position)
			{
				char_position = int_position;
			}
			if (func_c == NULL) {
				global_addr_map[variable->get_name()] = char_position;
			}
			else {
				offset_map[variable->get_name()] = char_position;
			}
			char_position += variable->get_len();
		}
		else if (variable->get_type() == INT) {
			if (char_position > int_position)
			{
				int_position = round_up(char_position, 4);
			}
			if (func_c == NULL)
			{
				global_addr_map[variable->get_name()] = int_position;
			}
			else
			{
				offset_map[variable->get_name()] = int_position;
			}
			int_position += 4 * variable->get_len();
		}
		else {
			error("error");
		}
	}
	else {
		if (variable->get_type() == CHAR) {
			if (char_position % 4 == 0 && int_position > char_position)
			{
				char_position = int_position;
			}
			if (func_c == NULL) {
				global_addr_map[variable->get_name()] = char_position;//recode the position of this global variable
			}
			else
			{
				offset_map[variable->get_name()] = char_position;
			}
			char_position += 1;
		}
		else if (variable->get_type() == INT) {
			if (char_position > int_position)
			{
				int_position = round_up(char_position, 4);
			}
			
			if (func_c == NULL)
			{
				global_addr_map[variable->get_name()] = int_position;
			}
			else
			{
				offset_map[variable->get_name()] = int_position;
			}
			//(cur_func == NULL ? global_addr_map : offset_map).insert(STRINT_MAP::value_type(var_item->get_name(), int_position));
			int_position += 4;
		}
		else {
			error("error");
		}
		

	}
		temp_base_addr = round_up((int_position > char_position) ? int_position : char_position, 4);//the next 4 space
		cur_addr = temp_base_addr;
}
void para_mips(string paraname) {
	para_read_count++;
	int addr = -para_read_count * 4;
	fout<<"lw $s" << get_reg(paraname,NULL) << ", " << addr << "($fp)"<<endl;
}
bool isType(string s) {
	return ((s == type2string(VOID)) || (s == type2string(CHAR)) || (s == type2string(INT)) );
}

void gen_func(string funcname) {
	offset_map.clear();

	map<string,FuncTableEntry* >::iterator each = functable.find(funcname);
	if (each != functable.end())
	{
		func_c= each->second;//get the function get the main?
	}
	else {
		func_c = NULL;
	}
	/*initialize*/
	temp_base_addr = 0;
	int_position = 0;
	char_position = 0;
	cur_addr = 0;
	para_read_count = 0;
	init_reg_map();
	fout<<funcname << "_label:"<<endl;
}
void call_mips(string funcname) {
	if (funcname != "main") {
		// store paras
		int len = get_func(funcname)->get_para_count();
		for (int i = 0; i < len; i++) {
			int addr = cur_addr + i * 4;
			string paraname = para_vector.back();
			para_vector.pop_back();

			if (is_num(paraname)) {
				fout<<"li $t0, " << paraname<<endl;
				fout<<"sw $t0, " << addr << "($fp)" << endl;
			}
			else {
				fout<<"sw $s" << get_reg(paraname,NULL) << ", " << addr << "($fp)" << endl;
			}
		}
		// save regs
		init_reg_map();

		fout<<"addi $sp, $sp, -8" << endl;
		fout<<"sw $ra, 0($sp)" << endl;
		fout<<"sw $fp, 4($sp)" << endl;

		// refresh $fp
		fout<<"addi $fp, $fp, " << round_up(cur_addr, 4) + len * 4 << endl;
		// jump
		fout<<"jal " << funcname << "_label" << endl;
		fout<<"nop" << endl;
		// load regs

		fout<<"lw $ra, 0($sp)" << endl;
		fout<<"lw $fp, 4($sp)" << endl;
		fout<<"addi $sp, $sp, 8" << endl;


	}
	else {//main函数
		// refresh $fp
		fout<<"addi $fp, $fp, " << round_up(cur_addr, 4) << endl;
		fout<<"add $fp, $fp, $gp" << endl;
		// jump
		fout<<"jal " << funcname << "_label" << endl;
		fout<<"nop" << endl;
		fout<<"li $v0, 10" << endl;
		fout<<"syscall" << endl;
	}
}
int get_reg(string name, vector<string>* conf_names) {
	bool is_tempname = is_temp(name);
	map<string, int>::iterator it;
	Type type = INT;

	if (!is_tempname) {
		if (func_c->has_var(name)) {
			type = func_c->search_var(name)->get_type();
		}
		else {

			map<string, VarTableEntry* >::iterator each = vartable.find(name);
			if (each != vartable.end())
			{
				type = each->second->get_type();
			}
			else {
				error("variable does not exist.");
			}
			
		}
	}

	it = reg_map.find(name);

	if (it != reg_map.end()) {
		return it->second;
	}
	else {
		bool keep_reg_state = false;
		if (conf_names != NULL)
		{
			int conf_len = conf_names->size();
			while (true)
			{
				bool conf = false;
				for (int i = 0; i < conf_len; i++)
				{
					if ((*conf_names)[i] == reg_regis[next_reg].name)
					{
						conf = true;
						break;
					}
				}
				if (conf)
				{
					next_reg = (next_reg + 1) % reg_count;
				}
				else
				{
					break;
				}
			}
		}
		Reg* regi = &reg_regis[next_reg];
		// store value
		if (regi->reg_state) {
			if (regi->global) {
				fout<<((regi->type== CHAR) ? "sb" : "sw") << " $s"
					<< next_reg << ", " << regi->offset << "($gp)"<<endl;
			}
			else {
				fout<<((regi->type == CHAR) ? "sb" : "sw") << " $s"
					<< next_reg << ", " << regi->offset << "($fp)"<<endl;
			}
			keep_reg_state = true;
		}
		else {
			keep_reg_state = false;
			regi->reg_state=true;
		}
		// record offset
		if (is_tempname) {
			int offset = temp_base_addr + get_temp_no(name) * 4;
			regi->offset = offset;
			if (offset + 4 > cur_addr) {
				cur_addr = offset + 4;
			}
			regi->global = false;
		}
		else {
			map<string, int>::iterator off_it = offset_map.find(name);
			if (off_it == offset_map.end()) {//not found
				off_it = global_addr_map.find(name);
				if (off_it == global_addr_map.end()) {
					error(name + " not exists");
				}
				else {
					regi->offset = off_it->second;
					regi->global = true;
				}

			}
			else {
				regi->offset = off_it->second;
				regi->global = false;
			}
		}
		// load value
		if (regi->global) { // is global variable
			fout<<((type == CHAR) ? "lb" : "lw") << " $s" << next_reg
				<< ", " << regi->offset << "($gp)"<<endl;
			
		}
		else {
			fout<<((type == CHAR) ? "lb" : "lw") << " $s" << next_reg
				<< ", " << regi->offset << "($fp)"<<endl;
		}

		regi->type = type;

		// erase old key
		string last_name = regi->name;
		if (keep_reg_state) {
			map<string, int>::iterator last_user = reg_map.find(last_name);
			if (last_user == reg_map.end()) {
				error(last_name + " not in map");
			}
			else {
				reg_map.erase(last_user);
			}
		}
		// insert new key
		regi->name=name;
		reg_map.insert(map<string, int>::value_type(name, next_reg));
		int return_reg = next_reg;
		next_reg = (next_reg + 1) % reg_count;
		return return_reg;
	}
}
void ass_cond(string op, string tar_str, string cal_str1, string cal_str2) {
	stringstream mips;
	bool is_cal = true;
	int imme1, imme2;
	bool is_imme1 = is_num(cal_str1);
	bool is_imme2 = is_num(cal_str2);
	vector<string> conf_names;
	conf_names.push_back(tar_str);
	conf_names.push_back(cal_str1);
	conf_names.push_back(cal_str2);
	if (is_imme1) {
		sscanf(cal_str1.c_str(), "%d", &imme1); 
	}
	if (is_imme2) {
		sscanf(cal_str2.c_str(), "%d", &imme2); 
	}

	if (op == "ADD") {
		mips << (is_imme2 ? "addi" : "add");
		is_cal = true;

	}
	else if (op == "SUB") {
		mips << (is_imme2 ? "addi" : "sub");
		if (is_imme2) imme2 = -imme2;   // turn negative
		is_cal = true;

	}
	else if (op == "MUL") {
		mips << "mul";
		is_cal = true;

	}
	else if (op == "DIV") {
		mips << "div";
		is_cal = true;

	}
	else if (op == "LE") { // <=
		mips << "sle";
		is_cal = false;

	}
	else if (op == "GE") {
		mips << "sge";
		is_cal = false;

	}
	else if (op == "LT") {
		mips << "slt";
		is_cal = false;

	}
	else if (op == "GT") {
		mips << "sgt";
		is_cal = false;

	}
	else if (op == "NE") {
		mips << "sne";
		is_cal = false;

	}
	else if (op == "EQ") {
		mips << "seq";
		is_cal = false;

	}
	else {
		error((string)"unknown op \'" + op + "\'");
	}

	if (is_num(tar_str)) {
		error("tar is number");
	}
	else {
		mips << " $s" << get_reg(tar_str, &conf_names);
	}

	if (is_imme1) {
		if (imme1 == 0) {
			mips << ", $0";
		}
		else if (!is_cal) {
			fout<<"li $t0, " << imme1<<endl;
			mips << ", $t0";
		}
		else {
			error("cal1 is a number");
		}
	}
	else {
		mips << ", $s" << get_reg(cal_str1, &conf_names);
	}


	if (is_imme2) {
		if (is_cal) {
			mips << ", " << imme2;
		}
		else {
			fout<<"li $t0, " << imme2<<endl;
			mips << ", $t0";
		}
	}
	else {
		mips << ", $s" << get_reg(cal_str2, &conf_names);
	}

	fout<<mips.str()<<endl;
}
void init() {
	fout << ".data" << endl;
	for (int i = 0; i < strs.size(); i++) {
		fout << "string" << i << ": .asciiz \"" << strs[i] << "\"" << endl;
	}

	fout<<".space 4"<<endl;
	fout<<"start_label:"<<endl;
	fout << ".text" << endl;
	fout<<"la $gp, " << "start_label"<<endl;
	fout<<"andi $gp, $gp, 0xFFFFFFFC"<<endl;//avoid the area of .data


}
void other_name(vector<string> a_strs) {//can not recognize
	
	if (a_strs.size() <= 1)
	{
		error("wrong name");
	}
	else if (a_strs[1] == ":")//a label
	{
		init_reg_map();
		fout<< a_strs[0] << ":"<<endl;
	}
	else if (a_strs.size() == 3&& a_strs[1] == "=")      // assign
	{
		vector<string> conf_names;
		conf_names.push_back(a_strs[0]);
		conf_names.push_back(a_strs[2]);
		if (is_num(a_strs[0])) {
			error("assign to number");
		}
		if (is_num(a_strs[2])) {
			// [MIPS] li
			int tmp = get_reg(a_strs[0], &conf_names);
			fout<<"li $s" << tmp << ", " << a_strs[2]<<endl;
			if (reg_regis[tmp].global) {
				fout << ((reg_regis[tmp].type == CHAR) ? "sb" : "sw") << " $s"
					<< tmp << ", " << reg_regis[tmp].offset << "($gp)" << endl;
			}

		}
		else {
			// [MIPS] move
			int tmp1 = get_reg(a_strs[0], &conf_names);
				
			fout<<"move $s" <<tmp1 << ", $s" << get_reg(a_strs[2], &conf_names)<<endl;
			if (reg_regis[tmp1].global) {
				fout << ((reg_regis[tmp1].type == CHAR) ? "sb" : "sw") << " $s"
					<< tmp1 << ", " << reg_regis[tmp1].offset << "($gp)" << endl;
			}
		}
	}
	else if (a_strs.size() == 5) {//assign and condition
		string str0 = a_strs[0];
		string str1 = a_strs[1];
		string str2 = a_strs[2];
		string str3 = a_strs[3];
		string str4 = a_strs[4];
		ass_cond(str3, str0, str2, str4);
	}
	else if (a_strs.size() == 6)      // array call function
	{
		string str0 = a_strs[0];
		string str1 = a_strs[1];
		string str2 = a_strs[2];
		string str3 = a_strs[3];
		string str4 = a_strs[4];
		string str5 = a_strs[5];
		if (str1 == "=")//get
		{
			array_mips(str2, str4, str0, false);
		}
		else if (str4 == "=")//set
		{
			array_mips(str0, str2, str5, true);
		}
		else {
			error("wrong function declarartion");
		}
		// immediate number? var(temp)?
	}

	else
	{
		error("too many parameters");
	}
}
VarTableEntry* get_var_item(string name)
{
	if (func_c->has_var(name))
	{
		return func_c->search_var(name);
	}
	else
	{
		return search_global_var(name);
	}
}

void array_mips(string arr_str, string off_str, string sou_str, bool is_set)
{
	VarTableEntry* item = get_var_item(arr_str);
	Type type;
	if (item == NULL) {
		error("unknown array \'" + arr_str + "\'");
	}
	else {
		type = item->get_type();
	}
	bool offset_is_imme = is_num(off_str);
	bool value_is_imme = is_num(sou_str);
	// get reg
	string reg;
	if (value_is_imme) {
		reg = "$t0";
		fout<<"li $t0, " << sou_str<<endl;
		if (!is_set) {
			error("array to a value");
		}
	}
	else {
		stringstream ss;
		ss << "$s" << get_reg(sou_str,NULL);
		reg = ss.str();
	}

	// get op
	string op;
	if (type == INT) {
		op = is_set ? "sw" : "lw";
	}
	else {
		op = is_set ? "sb" : "lb";
	}

	int offset;
	string point_reg;
	map<string,int>::iterator each = offset_map.find(arr_str);
	if (each != offset_map.end()) {
		offset = each->second;
		point_reg = "$fp";
	}
	else {
		each = global_addr_map.find(arr_str);
		if (each != global_addr_map.end()) {
			offset = each->second;
			point_reg = "$gp";
		}
		else {
			error("cannot found array");
		}
	}

	if (offset_is_imme) {
		int ele_offset;
		sscanf(off_str.c_str(), "%d", &ele_offset);
		if (type == INT) {
			ele_offset *= 4;
		}
		fout<<op << " " << reg << ", " << offset + ele_offset << "(" << point_reg << ")" << endl;

	}
	else {
		if (type == INT) {
			fout<<"sll $t1, $s" << get_reg(off_str,NULL) << ", 2" << endl;  // offset *= 4
			fout<<"add $t1, $t1, " << point_reg << endl;
		}
		else {
			fout<<"add $t1, $s" << get_reg(off_str,NULL) << ", " << point_reg << endl;
		}

		fout<<"addi $t1, $t1, " << offset << endl;  
		fout<<op << " " << reg << ", ($t1)" << endl;
	}
}
void each_mid() {
	string mid_code;
	while (getline(fin, mid_code)) {
		fout << "     #" << mid_code<<endl;
		istringstream each_str(mid_code);
		string str;
		vector<string> A_string;
		while (each_str >> str) {
			A_string.push_back(str);
		}
		if (A_string[0] == "#var" || A_string[0] == "#array")
		{
			if (func_c != NULL)   // in function
			{
				gen_var(func_c->search_var(A_string[2]));
			}
			else
			{
				gen_var(search_global_var(A_string[2]));
			}
		}
		else if (A_string[0] == "#para")
		{
			gen_var(func_c->search_var(A_string[2]));
			para_mips(A_string[2]);
		}
		else if (isType(A_string[0]))     // declaration of function
		{
			gen_func(A_string[1]);
		}
		else if (A_string[0] == "#label")
		{
			label_e = A_string[1];
			
		}
		else if (A_string[0] == "#push")     // parameter
		{
			para_vector.push_back(A_string[1]);
		}
		else if (A_string[0] == "#call")    //call function
		{
			call_mips(A_string[1]);
		}
		else if (A_string[0] == "#get")     // save register parameter
		{
			if (is_num(A_string[1])) {
				error("num get");
			}
			else {
				fout<<"move $s" << get_reg(A_string[1],NULL) << ", $v0"<<endl;
			}
			
		}
		else if (A_string[0] == "#ret")     // return
		{
			if (A_string.size() == 2 || A_string.size() == 1) {
				if (A_string.size() == 2)
							{
								if (is_num(A_string[1]))
								{
									fout<<"li $v0, " << A_string[1]<<endl;
								}
								else
								{
									fout<<"move $v0, $s" << get_reg(A_string[1], NULL)<<endl;
								}
							}
			
							fout<<"jr $ra"<<endl;
			}
			
			
		}
		else if (A_string[0] == "#BE")
		{
			init_reg_map();
			if (!is_num(A_string[2])) {
				error("be not num");
			}
			else {
				fout<<"beq $s" << get_reg(A_string[1],NULL) << ", " << A_string[2] << ", " << A_string[3]<<endl;
				fout<<"nop"<<endl;
			}
		}
		else if (A_string[0] == "#BZ")
		{
			init_reg_map();
			fout<<"beq $s" << get_reg(A_string[1],NULL) << ", $0, " << A_string[2]<<endl;
			fout<<"nop"<<endl;
		}
		else if (A_string[0] == "#BNZ")
		{
			init_reg_map();
			fout << "bne " << get_reg(A_string[1], false) << ", $0, " << A_string[2] << endl;
			fout<<"nop" << endl;
	
		}
		else if (A_string[0] == "#GOTO")
		{
			init_reg_map();
			fout<<"j " << A_string[1]<<endl;
			fout<<"nop" << endl;
		}
		
		else if (A_string[0] == "#printf")
		{

			bool is_immed = is_num(A_string[2]);
			if (A_string[1] == "string") {
				fout<<"li $v0, 4"<<endl;
				fout<<"la $a0, " << A_string[2] << endl;
				fout<<"syscall" << endl;
			}
			else if (A_string[1] == "int") {
				fout<<"li $v0, 1" << endl;
				if (is_immed) {
					fout<<"li $a0, " << A_string[2] << endl;
				}
				else {
					fout<<"move $a0, $s" << get_reg(A_string[2],NULL) << endl;
				}
				fout<<"syscall" << endl;

			}
			else if (A_string[1] == "char") {
				fout<<"li $v0, 11" << endl;
				if (is_immed) {
					fout<<"li $a0, " << A_string[2] << endl;
				}
				else {
					fout<<"move $a0, $s" << get_reg(A_string[2] ,NULL)<< endl;
				}
				fout<<"syscall" << endl;
			}
		}
		else if (A_string[0] == "#scanf")
		{
			if (A_string[1] == "int") {
				fout<<"li $v0, 5" << endl;
			}
			else if (A_string[1] == "char") {
				fout<<"li $v0, 12" << endl;
			}
			fout<<"syscall" << endl;
			fout<<"move $s" << get_reg(A_string[2],NULL) << ", $v0" << endl;
		}
		else if (A_string[0] == "#exit")
		{
			fout << "li $v0,10" << endl;
			fout << "syscall" << endl;
		}	
		else
		{
			other_name(A_string);
		}
	}

}
void mips_main() {
	string mips_filename;

	cout << "Enter the mips code filename: " << endl;
	cin >> mips_filename;
	fin.open(mid_filename.c_str());
	fout.open(mips_filename.c_str());
	init();
	each_mid();

	fout.close();
	fin.close();

}