#include <iostream>
#include <cstdlib>
#include <string.h>
#include "difi.h"
#include "syntax.h"
#include "mips.h"
#include<string> 
#define MAX_TOKEN_LENTH 1000
using namespace std;
FILE *File;


int main()
{
	string filename;
	string mid_filename;
	cout << "Enter the filename:" << endl;
	cin>>filename;

	File = fopen(filename.c_str(), "r");

	syn_main();
	mips_main();
	fclose(File);
	return 0;
}
