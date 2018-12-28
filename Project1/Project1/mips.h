#pragma once
#include "reg.h"
typedef struct {
	int offset = -1;
	string name = "";
	bool reg_state = false;
	bool global = false;
	Type type;
	
}Reg;
void mips_main();