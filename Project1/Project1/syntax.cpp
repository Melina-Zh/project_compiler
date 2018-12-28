#include<iostream>
#include<string>
#include <sstream>
#include<map>
#include <vector>
#include "syntax.h"
#include "difi.h"
#include "lexer.h"
#include "table.h"
#include "mid_code.h"
using  namespace std;
int flag=1;
bool is_func = false;

ofstream fout;
string name;
Type type;
int file_no=0;
int state_count;
FuncTableEntry* func_e;

ofstream f_mid;
map<ofstream,string> swi_filename;
/*table definition*/
map<string, VarTableEntry*> vartable;
map<string, ConstTableEntry*> consttable;
map<string, FuncTableEntry*> functable;
void compouding(FuncTableEntry* function);
Type expr(int* value, bool* is_cons, string* name);
bool defined(string name) {
	map<string, VarTableEntry*>::iterator each = vartable.find(name);
		if (each != vartable.end()) {
			error("redefined '"+name+"'");
			return true;
		}
	map<string, ConstTableEntry*>::iterator each1 = consttable.find(name);
		if (each1 != consttable.end()) {
			error("redefined '"+name+"'");
			return true;
		}
	map<string, FuncTableEntry*>::iterator each2 = functable.find(name);
		if (each2 != functable.end()) {
			error("redefined '"+name+"'");
			return true;
		}
		return false;
}
void print_info(string s) {
	//cout << s << endl;
}

FuncTableEntry* get_func(string name) {
	map<string, FuncTableEntry*>::iterator each = functable.find(name);
	if (each != functable.end()) return each->second;
	else return NULL;
}

ConstTableEntry* search_global_const(string name) {
	map<string, ConstTableEntry*>::iterator each = consttable.find(name);
	if (each == consttable.end()) {
		return NULL;
	}
	else {
		return each->second;
	}
}

VarTableEntry* search_global_var(string name) {
	map<string, VarTableEntry*>::iterator each = vartable.find(name);
	if (each == vartable.end()) {//not found
		return NULL;
	}
	else {
		return each->second;
	}
}

TableEntry* search_table(string name) {
	TableEntry* entry;
	if ((entry = func_e->search_cons(name)) != NULL) return entry;
	if ((entry = func_e->search_var(name)) != NULL) return entry;
	if ((entry = search_global_const(name)) != NULL) return entry;
	if ((entry = search_global_var(name)) != NULL) return entry;
	if ((entry = get_func(name)) != NULL) return entry;
	return NULL;
}

void skip(Symbol tar_sym)
{
	while (symbol != tar_sym)
	{
		if (!getsym()) exit(0);
	}
	if (!getsym()) exit(0);
}
bool is_match(Symbol sym) {
	
	if (symbol != sym) {
		if ((sym == ZERO && symbol == INTCON) || (sym == INTCON && symbol == ZERO)) {
			if (token_top != 0 && sym == IDENT) {
				token[token_top] = 0;
				name = token;
			}

			if (!getsym()) return false;
			return true;
		}
		error((string)"got " + symbol2string(symbol) + " expected " + symbol2string(sym));
		return false;
	}
	else {
		if (token_top != 0&&sym==IDENT) {
			token[token_top] = 0;
			name = token;
		}
		
		if (!getsym()) exit(0);
		return true;
	}
}Type factor(int* value,bool* is_cons, string* name) {
	Type tp;
	TableEntry*entry;
	string* index_name=new string();
	int len;
	switch (symbol) { 
	case IDENT:
	{
		bool have_par = false;
		*name = token;
		if ((entry = search_table(*name)) == NULL) {
			error(*name + " NOT FOUND!");
			*is_cons = true;
			*value = 0;
			return CHAR;
		}
		if (!getsym()) exit(0);
		if (entry->get_kind() == VAR && ((VarTableEntry*)entry)->isarray() && symbol != LBKT)
		{
			error((string)"array " + *name + " without an index");
		}
		/*entry is the founded entry, symbol is the next one*/
		/*if an array*/
		if (symbol == LBKT) {//'['
			if (!getsym()) exit(0);
			if (entry->get_kind() != VAR || !((VarTableEntry*)entry)->isarray()) {
				error((string)*name + "  is not an array type");
				skip(RBKT); // [ERROR HANDLE] skip to ']'
			}
			int index_value;
			bool index_is_cons;
			
			expr(&index_value, &index_is_cons, index_name);
			len = ((VarTableEntry*)entry)->get_len();
			if (index_is_cons && (index_value < 0 || index_value >= len))
			{
				error((string)"index of array \'" + *name + "[]\' out of range");
				index_value = 0;    // mistake handling
			}
			if (index_is_cons)
			{
				*index_name = create_tmp();
				assign_mid(*index_name, index_value);
			}
			is_match(RBKT);
		}

		/*is a function, paralist*/
		else if (symbol == LPAR) {//'('
			have_par = true;
			vector<Type> para_list;
			if (!getsym()) exit(0);
			do {
				int para_value;
				bool para_is_cons;
				string* para_name= new string();

				if (symbol == RPAR)
					break;
				para_list.push_back(expr(&para_value, &para_is_cons, para_name));//add
				if (para_is_cons) {
					push_mid(para_value);
				}
				else {
					push_mid(*para_name);
				}

				if (symbol != COMMA) {
					break;
				}
				if (!getsym()) exit(0);
			} while (true);
			if (entry->get_kind() != FUNC || !((FuncTableEntry*)entry)->check_para(para_list)) {
				error("this parameter to function" + entry->get_name() + " is not exist");
			}
			is_match(RPAR);
		}
		tp = entry->get_type();
		if (entry->get_type() == VOID) {
			error("void is not accepted.");
			*is_cons = true;
			*value = 0;
			return CHAR;
		}
		if (entry->get_kind() == CONST) {
			*value = ((ConstTableEntry*)entry)->get_value();
			*is_cons = true;
		}
		else if (entry->get_kind() == VAR) {
			if (((VarTableEntry*)entry)->isarray())
			{
				*name = create_tmp();
				array_get_mid(*name,entry->get_name(), *index_name);
			}
			else
			{
				
				*name = entry->get_name();

			}
			*is_cons = false;
		}
		else if(entry->get_kind() == FUNC){
			if (!have_par) {
				error("It is supposed to have parentheses when calling function");
			}
			func_call_mid(entry->get_name());
			*name = create_tmp();
			return_get_mid(*name);
			*is_cons = false;

		}
	}
		break;
	case LPAR://an expression
		if (!getsym()) exit(0);
		expr(value,is_cons,name);
		tp = INT;
		is_match(RPAR);
		break;
	case ADD:
		if (!getsym()) exit(0);
		tp = INT;
		if (is_match(INTCON)) {
			*value = number;
		}
		else {
			*value = 0;
		}
		*is_cons = true;
		break;
	case SUB:
		if (!getsym()) exit(0);
		tp = INT;
		if (is_match(INTCON)) {
			*value = (-1)*number;
		}
		else {
			*value = 0;
		}
		*is_cons = true;
		break;
	case INTCON:
		tp = INT;
		*value = number;
		*is_cons = true;
		if (!getsym()) exit(0);
		break;
	case CHARCON:
		tp = CHAR;
		*value = number;
		*is_cons = true;
		if (!getsym()) exit(0);
		break;
	case ZERO:
		tp = INT;
		*value = 0;
		*is_cons = true;
		if (!getsym()) exit(0);
		break;
	default:
		error((string)"unexpected token \'" + symbol2string(symbol) + "\' in factor");
		*value = 0;
		*is_cons = true;
		return CHAR;

	}
	return tp;
}

Type item(int* value,bool* is_cons,string* name) {
	*value = 1;
	*is_cons = true;
	Type tp = CHAR;
	Symbol op = MUL;//onlt mul and div between factors
	int fac_value;
	bool fac_is_cons;
	string* fac_name= new string();
	bool is_left = true;
	do {
		if (factor(&fac_value, &fac_is_cons, fac_name) == INT) {
			tp = INT;
		}
		if (!fac_is_cons) {
			*is_cons = false;
		}
		if (op == DIV && fac_is_cons&&fac_value == 0) {
			error("fatal: Divided by zero");
			fac_value = 1;//mistake handling
		}

		if (*is_cons&&fac_is_cons) {
			if (op == MUL) {
				*value *= fac_value;
			}
			else if (op == DIV) {
				*value /= fac_value;
			}
			else {
				error("Something wrong in item.");
			}
		}
		else if (is_left) {
			is_left = false;
			*name = create_tmp();
			if (*value == 1 && op == MUL) {
				assign_mid(*name, *fac_name);
			}
			else {
				assign_mid(*name, *value);
				cond_mid(op, *name, *name, *fac_name);
			}
		}
		else if (!fac_is_cons) {
			cond_mid(op, *name, *name, *fac_name);
		}
		else {
			cond_mid(op, *name, *name, fac_value);
		}
		if (symbol == MUL || symbol == DIV) {
			op = symbol;
			tp = INT;
			if (!getsym()) exit(0);
		}
		else {
			break;
		}
	} while (true);
	return tp;
}

Type expr(int* value,bool* is_cons,string* name) {
	Symbol op=ADD;
	int item_value;
	bool item_is_cons;
	string* item_name=new string();
	bool is_left=true;
	Type tp = CHAR;
	*value = 0;
	*is_cons = true;

	if (symbol == ADD || symbol == SUB) {
		tp = INT;
		op = symbol;
		if (!getsym()) exit(0);
	}
	do {
		if (item(&item_value,&item_is_cons,item_name)==INT) {
			tp = INT;
		}
		if (!item_is_cons) {
			*is_cons = false;
		}
		*is_cons = *is_cons&item_is_cons;
		if (*is_cons&&item_is_cons) {// is a constant and it could accumulate
			if (op == ADD ) {
//if (!getsym()) exit(0);
				*value += item_value;
			}
			else if(op ==SUB){
				*value -= item_value;
			}
			else {
				error("Something wrong in expression.");
			}
		}
		else if (is_left) {
			is_left = false;
			*name = create_tmp();
			if (*value == 0) {
				if (op == ADD) {
					assign_mid(*name, *item_name);
				}
				else {
					cond_mid(op, *name, 0, *item_name);
				}
			}
			else {
				assign_mid(*name, *value);
				cond_mid(op, *name, *name, *item_name);
			}
		}
		else if (!item_is_cons) {//generally, the second one, is a variable
			cond_mid(op, *name, *name, *item_name);
		}
		else {
			cond_mid(op, *name, *name, item_value);//the second one, is a constant.
		}
		if (symbol == ADD || symbol == SUB) {
			op = symbol;
			tp = INT;
			if (!getsym()) exit(0);
		}
		else {
			break;
		}

	} while (true);
	print_info((string)"This is an expression statement");
	return tp;
}


string condition(int* value,bool* is_cons) {
	string* left_name= new string();
	string* right_name= new string();
	string c_name;
	Symbol op;
	int left_value;
	int right_value;
	bool left_is_cons;
	bool right_is_cons;
	expr(value,is_cons,left_name);
	
	switch (symbol) {
	case GT:
	case GE:
	case LT:
	case LE:
	case EQ:
	case NE:
		op = symbol;
		if (!getsym()) exit(0);
		break;
	default:
		c_name = *left_name;//just one expression as the condition
		print_info((string)"This is a condition");
		return c_name;
	}
	left_is_cons = *is_cons;
	left_value = *value;
	

	expr(value,is_cons,right_name);
	right_is_cons = *is_cons;
	right_value = *value;
	

	if (left_is_cons && right_is_cons)   // the condition is a constant
	{
		switch (op)
		{
		case GT:
			*value = (left_value > right_value);
			break;
		case GE:
			*value = (left_value >= right_value);
			break;
		case LT:
			*value = (left_value < right_value);
			break;
		case LE:
			*value = (left_value <= right_value);
			break;
		case EQ:
			*value = (left_value == right_value);
			break;
		case NE:
			*value = (left_value != right_value);
			break;
		default:
			error("Something is wrong in condition");
		}
		return c_name;

	}
	else if (left_is_cons && !right_is_cons)       // right is a variable
	{
		cond_mid(op, *right_name, left_value,*right_name);
		c_name = *right_name;
	}
	else if (!left_is_cons && right_is_cons)
	{
		cond_mid(op, *left_name, *left_name, right_value);
		c_name = *left_name;
	}
	else
	{
		cond_mid(op, *left_name, *left_name, *right_name);//both variable
		c_name = *left_name;
	}
	*is_cons = false;
	return c_name;

}
void statement() {
	string* index_name = new string();
	switch (symbol) {
	case IFSY: { 
		string end_if;//a label
		int cond_value;//zero or not
		string cond_name = "";
		bool cond_is_cons;
		end_if = creat_label(func_e, "end_if");
		if (!getsym()) exit(0);
		is_match(LPAR); // '('

		cond_name = condition(&cond_value, &cond_is_cons); // identify condition
		is_match(RPAR); // ')'

		print_info((string)"This is an if statement");
		if (cond_is_cons && cond_value == 0)
		{
			branch_without_mid(end_if);
		}
		else if (!cond_is_cons)
		{

			branch_not_mid(cond_name, end_if);
		}

		statement();    // statement among if
		label_mid(end_if);
		break;
	}
	case WHILESY:
	{
		string while_begin = creat_label(func_e, "while_begin");
		string while_end = creat_label(func_e, "while_end");
		if (!getsym()) exit(0);
		is_match(LPAR); // '('
		label_mid(while_begin);//set begin label
		int cond_value;
		bool cond_is_cons;
		string* cond_name = new string();
		*cond_name = condition(&cond_value, &cond_is_cons); // identify condition

		is_match(RPAR); // ')'
		if (cond_is_cons&&cond_value == 0) {
			branch_without_mid(while_end);
		}
		else if (!cond_is_cons) {
			branch_not_mid(*cond_name, while_end);
		}
		statement();    // statement among if
		branch_without_mid(while_begin);
		print_info((string)"This is a while statement");
		label_mid(while_end);
		break;
	}

	case LBRACE:
		if (!getsym()) exit(0);
		//branchs.push_back(0);
		while (symbol != RBRACE)
		{
			statement();
			if (end()) {
				error("It is supposed to have a RBRACE.");
				return;
			}
			
		} // '}'

	
		if (!getsym()) exit(0);
		break;
	case IDENT:
		//if (!getsym()) exit(0);
	{
		Type tp;
		TableEntry*entry;
		bool have_par_state = false;
		int len;
		name = token;
		if ((entry = search_table(name)) == NULL) {
			error(name + " NOT FOUND!");
			if (!getsym()) exit(0);
			break;
		}
		if (!getsym()) exit(0);
		if (entry->get_kind() == VAR && ((VarTableEntry*)entry)->isarray() && symbol != LBKT)
		{
			error((string)"array " + name + " without an index");
		}
		/*entry is the founded entry, symbol is the next one*/
		/*if an array*/
		if (symbol == LBKT) {//'['
			if (!getsym()) exit(0);
			if (entry->get_kind() != VAR || !((VarTableEntry*)entry)->isarray()) {
				error((string)name + "  is not an array type");
				skip(RBKT); // [ERROR HANDLE] skip to ']'
			}
			int index_value;
			bool index_is_cons;

			expr(&index_value, &index_is_cons, index_name);
			len = ((VarTableEntry*)entry)->get_len();
			if (index_is_cons && (index_value < 0 || index_value >= len))
			{
				error((string)"index of array \'" + name + "[]\' out of range");
				index_value = 0;    // mistake handling
			}
			if (index_is_cons)
			{
				*index_name = create_tmp();
				assign_mid(*index_name, index_value);
			}
			is_match(RBKT);
		}

		/*is a function, paralist*/
		else if (symbol == LPAR) {//'('
			have_par_state = true;

			vector<Type> para_list;
			if (!getsym()) exit(0);
			do {
				int para_value;
				bool para_is_cons;
				string* para_name = new string();

				if (symbol == RPAR)
					break;
				para_list.push_back(expr(&para_value, &para_is_cons, para_name));//add
				if (para_is_cons) {
					push_mid(para_value);
				}
				else {
					push_mid(*para_name);
				}

				if (symbol != COMMA) {
					break;
				}
				if (!getsym()) exit(0);
			} while (true);
			if (entry->get_kind() != FUNC || !((FuncTableEntry*)entry)->check_para(para_list)) {
				error("this parameter to function" + entry->get_name() + " is not exist");
			}
			is_match(RPAR);
		}
		if (symbol == SEMI)
		{
			if (entry->get_kind() != FUNC) {
				error("It is not a function.");
			}
			else {
				if (!have_par_state) {
					error("It is supposed to have parentheses when calling function");
				}
				
				func_call_mid(entry->get_name());
			}
			print_info((string)"This is a function calling statement!");

		}
		else if (symbol == ASS)
		{
			if (!getsym()) exit(0);
			print_info((string)"This is an assign statement!");
			if (entry->get_kind() != VAR)
			{
				error((string)"assign to a non-variable \'" + entry->get_name() + "\'");
				skip(SEMI);
				return;
			}
			int ass_value;
			bool ass_is_cons;
			string* ass_name = new string();
			Type ty_left,ty_right;
			ty_left = expr(&ass_value, &ass_is_cons, ass_name);
			ty_right = entry->get_type();
			if (ty_left != ty_right ) {
				error("can't assign "+type2string(ty_left)+" to " +type2string(ty_right));
			}
			

			if (ass_is_cons)
			{
				if (((VarTableEntry*)entry)->isarray())
				{
					array_set_mid(entry->get_name(), *index_name, ass_value);
				}
				else
				{
					assign_mid(entry->get_name(), ass_value);
				}
			}
			else
			{
				if (((VarTableEntry*)entry)->isarray())
				{
					array_set_mid(entry->get_name(), *index_name, *ass_name);
				}
				else
				{
					assign_mid(entry->get_name(), *ass_name);
				}
			}

		}
		is_match(SEMI); // ';'

		break;
	}
	case SCFSY:
		print_info((string)"This is a read statement!");
		if (!getsym()) exit(0);
		is_match(LPAR);
		do
		{
			if (is_match(IDENT))
			{
				
				TableEntry* entry;
				if ((entry = search_table(name)) == NULL) {
					error(name + " NOT FOUND!");
				}
				else if (entry->get_kind() != VAR || ((VarTableEntry*)entry)->isarray())
				{
					error("can only write to variables");
				}
				else
				{
					scanf_mid(entry->get_type(), entry->get_name());
				}
				

				if (symbol != COMMA)   // ','
				{
					break;
				}
			}
			else
			{
				if (symbol == SEMI)
				{
					break;
				}
			}
			if (!getsym()) exit(0);
		} while (true);
		is_match(RPAR);
		is_match(SEMI);
		break;
	case PRTFST: {
		bool is_expr = false;
		bool is_str = false;
		string s;
		if (!getsym()) exit(0);
		is_match(LPAR); // '('
		if (symbol == STRCON)   // string
		{
			token[token_top] = 0;
			s = token;
			is_str = true;
			if (!getsym()) exit(0);
			if (symbol == COMMA)    // ','
			{
				if (!getsym()) exit(0);
				is_expr = true;
			}
		}
		else
		{
			
			is_expr = true;
		}
		if (is_str && !is_expr)
		{
			print_mid(STRING, s);
			print_info((string)"This is a print statement!");

		}
		if (is_expr)
		{
			string* print_name= new string();
			int print_value;
			bool print_is_cons;
			Type tp;
			tp = expr(&print_value, &print_is_cons, print_name);
			if (is_str) {
				print_mid(STRING, s);
			}
			if (print_is_cons) {
				print_mid(tp, print_value);
			}
			else {
				print_mid(tp, *print_name);
			}
			print_info((string)"This is a print statement!");
		}
		is_match(RPAR); // ')'
		is_match(SEMI);
		break; }
	case RTNSY:
	{
		Type tp;
		if (!getsym()) exit(0);
		if (symbol != SEMI)
		{
			is_match(LPAR);
			int return_value;
			bool return_is_cons;
			string* return_name= new string();
			tp = expr(&return_value, &return_is_cons, return_name);
			if (tp != func_e->get_type())
			{
				error("/NON-MATCHED/: The return type dismatched the function type.");
			}
			if (return_is_cons)
			{
				return_mid(return_value);
			}
			else
			{
				return_mid(*return_name);
			}
			is_match(RPAR);
			is_match(SEMI);
			print_info((string)"This is a return statement!");

		}
		else//return withour value
		{
			if (func_e->get_type() != VOID) {
				error("It is supposed to have a return value in /" + func_e->get_name() + " /");
			}
			return_mid(func_e);
			print_info((string)"This is a return statement!");
			if (!getsym()) exit(0);
		}
		break; }
	case SWTSY:
	{
		string* switch_name= new string();
		
		bool switch_is_cons;
		int switch_value;
		map<int, string>label_map;//many cases
		if (!getsym()) exit(0);
		string switch_end = creat_label(func_e, "switch_end");
		is_match(LPAR); // '('
		
		type=expr(&switch_value,&switch_is_cons,switch_name);
		is_match(RPAR); // ')'
		is_match(LBRACE);   // '{'
		is_match(CASESY);   // case
		
		do
		{
			string label = creat_label(func_e, "case");
			string label_over = creat_label(func_e, "case_over");
			int sign = 1;
			print_info((string)"Case statement begins!");
			switch (symbol)
			{
			case ZERO:
			case INTCON:      // int
			{
				if (type == CHAR) {
					error("/NON-MATCHED/: expected CHAR, got INT");
				}
				if (!getsym()) exit(0);
				break;
			}
			case CHARCON:   // char
			{
				if (type == INT) {
					error("/NON-MATCHED/: expected INT, got CHAR");
				}
				if (!getsym()) exit(0);
				break;
			}
			case SUB: {
				sign = -1;
			}

			case ADD:
				if (!getsym()) exit(0);
				is_match(INTCON);
				break;

			default:
				error((string)"unexpected token \'" + symbol2string(symbol) + "\' after [case]");
			}
			number *= sign;    
			
			if (switch_is_cons) {
		
					if (switch_value == number) {
						branch_without_mid(label);
					}
					else {
						branch_without_mid(label_over);
					}
					
			}
			else {
				
				branch_equal_mid(*switch_name, number, label);//go to the case
				branch_without_mid(label_over);

			}

			is_match(COLON);    // :
			
			if (label_map.find(number) != label_map.end()) {
				error("This case has been defined above.");
			}
			else {
				label_map.insert(map<int, string>::value_type(number, label));
			}
			label_mid(label);//set label
			statement();
			branch_without_mid(switch_end);
			label_mid(label_over);
			if (symbol != CASESY)
			{
				break;
			}
			if (!getsym()) exit(0);
		} while (true); 
		string default_label;
		if (symbol == DFTSY)
		{
			default_label = creat_label(func_e, "default");
			print_info((string)"Default statement begins!");
			if (!getsym()) exit(0);
			is_match(COLON);    // :
			label_mid(default_label);
			statement();
			branch_without_mid(switch_end);
		}
		is_match(RBRACE);   // '}'
		print_info((string)"Switch statement over!");

		label_mid(switch_end);
		break; }
	case SEMI:
		if (!getsym()) exit(0);
		break;
	default:
		error((string)"unexpected token \'"+ symbol2string(symbol) +"\'");
		if (!getsym()) exit(0);
		break;
	}

}

void declar_const(FuncTableEntry* function=NULL) {
	bool wrong = false;
	while (symbol == CONSTSY) {
		if (!getsym()) exit(0);
		switch (symbol) {
		case INTSY:
			if (!getsym()) exit(0);
			do {
				if (is_match(IDENT)) {
					
 					if (is_match(ASS))
						flag = 1;
					if (symbol == ADD) {
						flag = 1;
						if (!getsym()) exit(0);
					}
					else if (symbol == SUB) {
						flag = -1;
						if (!getsym()) exit(0);
					}
					if (symbol == INTCON || symbol == ZERO) {
						if (!getsym()) exit(0);
					}
					if (function == NULL) {
						if (defined(name))return;//redefinition
						//global constant-->consttable
						consttable.insert(map<string, ConstTableEntry*>::
							value_type(name, new ConstTableEntry(name, INT, flag*number)));
					}
					else {
						function->add_const(name, INT, flag*number);
					}
					if (symbol != COMMA) {
						break;
					}

				}
				else {
					if (symbol == SEMI) {
						print_info((string)"This is a constant daclaration.");
						break;
					}
				}
				if (!getsym()) exit(0);
			} while (true);
			break;
		case CHARSY:
			if (!getsym()) exit(0);
			do {
				if (is_match(IDENT)) {
					
					is_match(ASS);

					if (!is_match(CHARCON))
					{
						wrong = true;
						skip(SEMI);//handle mistakes

					}
					
					if (function == NULL) {
						if (defined(name) || wrong) {
							wrong = false;
							break;//redefinition
						}
						//global constant-->consttable
						consttable.insert(map<string, ConstTableEntry*>::
							value_type(name, new ConstTableEntry(name, CHAR, number)));
					}
					else {
						function->add_const(name, CHAR, number);
					}
					if (symbol != COMMA) {
						break;
					}

				}
				else {
					if (symbol == SEMI) {
						print_info((string)"This is a constant daclaration.");
						break;
					}
				}
				if (!getsym()) exit(0);
			} while (true);
			break;
		default:
			error((string)"unexpected const type " + symbol2string(symbol));
			skip(SEMI);
			return;
		}
		if (is_match(SEMI)){
			print_info((string)"This is a constant declaration.");
			
		}
	}
}
void declar_var(FuncTableEntry* function = NULL) {
	while (true) {
		switch (symbol) {
		case CONSTSY:
			error((string)"constant declaration is supposed to be in front.");
			skip(SEMI);
			continue;
		case INTSY:
			type = INT;
			break;
		case CHARSY:
			type = CHAR;
			break;
		default:
			return;
		}
		if (!getsym()) exit(0);//read the identifier
		do {
			if (is_match(IDENT)) {
				
				if (symbol == LBKT)//'[' an array
				{
					if (!getsym()) exit(0);
					if (symbol == ZERO) {
						error("the size of an array must >0");
					}
					is_match(INTCON);
					
					if (function == NULL) {
						if (defined(name))return;//redefinition
						//global array-->vartable
						    VarTableEntry* var_e = new VarTableEntry(name, type, number);
							vartable.insert(map<string, VarTableEntry*>::
								value_type(name,var_e));
							var_declar_mid(var_e);
					}
					else {
						function->add_var(name, type, number);
					}
					is_match(RBKT); // ']'

				}
				else if (symbol == LPAR) {
					is_func = true;
					return;//function with return value
				}
				else
				{
					//not an array.
					if (function == NULL) {
						if (defined(name))return;//redefinition
						//global variable-->vartable
						VarTableEntry* var_e = new VarTableEntry(name, type, 0);//a variable initialize
						vartable.insert(map<string, VarTableEntry*>::
							value_type(name, var_e));
						var_declar_mid(var_e);
					}
					else {
						function->add_var(name, type);//default value
					}
				}

				if (symbol != COMMA)   // ','
				{
					break;
				}
			}
			else {
				if (symbol == SEMI) {
					print_info((string)"This is a variable declaration.");
					break;
				}

			}
			if (!getsym()) exit(0);
		} while (true);
		if (is_match(SEMI)) {
			print_info((string)"This is a variable declaration.");
		}
	}
}
void declar_func() {

	while (true) {
		if (is_func) {
			is_func = false;//has already read some characters of this function
		}
		else {
			switch (symbol) {
			case VOIDSY:
				if (!getsym()) exit(0);
				if (symbol == MAINSY) {
					if (!getsym()) exit(0);
					is_match(LPAR);
					is_match(RPAR);
					func_e = new FuncTableEntry("main", VOID);
					functable.insert(map<string, FuncTableEntry*>::value_type
					("main", func_e));
					print_info((string)"This is the main function declaration.");
					func_declar_mid(func_e);
					compouding(func_e);
					return;
				}
				else {
					type=VOID;
					//a function without return value
				}
				break;
			case INTSY:
				if (!getsym()) exit(0);
				type = INT;
				break;
			case CHARSY:
				if (!getsym()) exit(0);
				type = CHAR;
				break;
			case CONSTSY:
				error((string)"constant declaration is supposed to be in front.");
				skip(SEMI);
				continue;
			default:
				error((string)"invalid returning type " + symbol2string(symbol));
				if (!getsym()) exit(0);

			}
			if (is_match(IDENT)) {
				;
			}
		}
		/*declaration of function, reloading function rejected*/
		if (!defined(name)) {
			func_e = new FuncTableEntry(name, type);
			func_declar_mid(func_e);
			functable.insert(map<string, FuncTableEntry*>::value_type(name, func_e));
		}
		if (symbol == SEMI) {
			error((string)"var defination after function");
			if (!getsym())
			{
				error("wrong program");
			}
			continue;
		}
		if (symbol == LPAR)//parameter list
		{
			if (!getsym()) exit(0);
			do
			{
				switch (symbol)
				{
				case INTSY:
					if (!getsym()) exit(0);
					type = INT;
					break;
				case CHARSY:
					if (!getsym()) exit(0);
					type = CHAR;
					break;
				case RPAR://has no parameter
					break;
				default:
					error((string)"invalid parameter type " + symbol2string(symbol));
				}
				if (symbol == IDENT)
				{
					token[token_top] = 0;
					name = token;
					func_e->add_para(name, type);
					if (!getsym()) exit(0);

					if (symbol != COMMA)   // ','
					{
						break;
					}
				}
				else
				{
					if (symbol == RPAR)
					{
						break;
					}
				}
				if (!getsym()) exit(0);
			} while (true);
			is_match(RPAR);
		}
		else if(symbol==LBRACE){
			error("It is supposed to have parentheses when declaring function");
		}
		compouding(func_e);

	}
}

void compouding(FuncTableEntry* function) {
	is_match(LBRACE);
	declar_const(function);
	declar_var(function);
	state_count = 0;
	//calculate space
	while (symbol != RBRACE) {
		statement();
		
	}

	return_mid(function);
	
	if (function->get_name() == "main") {
		if (current_char == EOF) {
			return;
			}

			if(!read_char_main())//not end
			error("There is something behind 'main'function.");
			return;
		}

	
	else {
		if (!getsym())
		{
			error("'main'function is needed.");
		}
	}
}

void syn_main() {

	if (!getsym()) {
		error((string)"wrong program");
		exit(0);
	}
	
	
	cout << "Enter the intermediate code filename: " << endl;
	cin >>mid_filename;

	fout.open(mid_filename.c_str());
	//begin with program
	//cout << "<<get constants>>" << endl;
	declar_const(NULL);
	//cout << "<<get variable>>" << endl;
	declar_var(NULL);
	func_call_mid("main");
	exit_mid(); 
	//cout << "<<get function>>" << endl;
	declar_func();

	fout.close();

}