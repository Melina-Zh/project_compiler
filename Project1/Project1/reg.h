/*#pragma once
#include <string>
#include "difi.h"
typedef enum
{
	ACTIVE,
	INACTIVE // use diregisttly (not be modified)
} State;
class Reg {
private:
	string reg_name;
	State state;
public:
	bool global = false;
	Type type;
	int use_count = 0;
	int offset = -1;
	Reg();
	State get_state();
	void set_state(State s);
	string get_name();
	void set_name(string s);
	
};*/