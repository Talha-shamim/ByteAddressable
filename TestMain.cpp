#include<iostream>
#include"Assembler.h"
#include<sstream>
#include<string>
#include<fstream>
using namespace std;
int main() 
{
	string filename;
	cout << "Enter the filename" << endl;
	cin >> filename;
	ifstream f(filename); 
	string str;
	if(f) 
	{
		ostringstream ss;
		ss << f.rdbuf();
		str = ss.str();
	}
	else
	{
		cout << "file not found" << endl;
		return 0;
	}
	registersSet.insert({"zero", 0});
	registersSet.insert({"at", 1});
	registersSet.insert({"v0", 2});
	registersSet.insert({"v1", 3});
	registersSet.insert({"a0", 4});
	registersSet.insert({"a1", 5});
	registersSet.insert({"a2", 6});
	registersSet.insert({"a3", 7});
	registersSet.insert({"t0", 8});
	registersSet.insert({"t1", 9});
	registersSet.insert({"t2", 10});
	registersSet.insert({"t3", 11});
	registersSet.insert({"t4", 12});
	registersSet.insert({"t5", 13});
	registersSet.insert({"t6", 14});
	registersSet.insert({"t7", 15});
	registersSet.insert({"s0", 16});	
	registersSet.insert({"s1", 17});
	registersSet.insert({"s2", 18});
	registersSet.insert({"s3", 19});
	registersSet.insert({"s4", 20});
	registersSet.insert({"s5", 21});
	registersSet.insert({"s6", 22});
	registersSet.insert({"s7", 23});
	registersSet.insert({"t8", 24});
	registersSet.insert({"t9", 25});
	registersSet.insert({"k0", 26});
	registersSet.insert({"k1", 27});
	registersSet.insert({"gp", 28});
	registersSet.insert({"sp", 29});
	registersSet.insert({"s8", 30});
	registersSet.insert({"ra", 31});

	Assembler p;
	int parsingerror = p.parse( str , 0 );
	reg.setRegister( 31 , 8172 );
	reg.setRegister( 29 , 4096 );
	if( parsingerror != 1 )
	{
		cout << "Parsing error" << endl;
		return 0;
	}
	cout << "Parsing complete" << endl;
	mem.WriteInstructions();

	char option;
	bool stepWiseExecution;
	bool pipelineOrNot;
	bool operandForwardingOrNot;
	bool earlyBranchResolutionOrNot;
	bool cacheEnabledOrNot;
	bool branchPredictorOrNot;
	bool ext;
	int l1 = 16;
	int l2 = 32;
	int b = 4;
	int a = 1;
	int l1time = 1;
	int l2time = 1;
	int memorytime = 1;
	int buffertime = 1;
	int replacementPolicy = 0;
step:
	cout << "Enter 1 for Stepwise Execution and 0 for Normal Execution" << endl;
	cin >> option;
	if( option == '1' )
	{
		stepWiseExecution = true;
		pipelineOrNot = false;
		operandForwardingOrNot = false;
		earlyBranchResolutionOrNot = false;
		branchPredictorOrNot = false;
		goto cachee;
	}
	else if( option == '0' )
		stepWiseExecution = false;
	else
	{
		cout << "Enter correct choice" << endl;
		goto step;
	}

pipeline:
	cout << "Enter 1 for Pipelined Execution and 0 for Unpipelined Execution" << endl;
	cin >> option;
	if( option == '1' )
	{
		pipelineOrNot = true;
	}
	else if( option == '0' )
	{
		pipelineOrNot = false;
		operandForwardingOrNot = false;
		earlyBranchResolutionOrNot = false;
		branchPredictorOrNot = false;
		goto cachee;
	}
	else
	{
		cout << "Enter correct choice" << endl;
		goto pipeline;
	}

operandforwarding:
	cout << "Enter 1 to enable OperandForwarding and 0 to disable OperandForwarding" << endl;
	cin >> option;
	if( option == '1' )
	{
		operandForwardingOrNot = true;
	}
	else if( option == '0' )
	{
		operandForwardingOrNot = false;
	}
	else
	{
		cout << "Enter correct choice" << endl;
		goto operandforwarding;
	}

earlybranch:
	cout << "Enter 1 to enable Early Branch Resolution and 0 to disable it" << endl;
	cin >> option;
	if( option == '1' )
	{
		earlyBranchResolutionOrNot = true;
	}
	else if( option == '0' )
	{
		earlyBranchResolutionOrNot = false;
	}
	else
	{
		cout << "Enter correct choice" << endl;
		goto earlybranch;
	}

branchpredictor:
	cout << "Enter 1 to enable BranchPredictor and 0 to disable it" << endl;
	cin >> option;
	if( option == '1' )
		branchPredictorOrNot = true;
	else if( option == '0' )
		branchPredictorOrNot = false;
	else
	{
		cout << "Enter correct choice" << endl;
		goto branchpredictor;
	}

cachee:
	cout << "Enter 1 to enable Cache and 0 to disable it" << endl;
	cin >> option;
	if( option == '1' )
	{
		cacheEnabledOrNot = true;
		cout << "Enter the size of L1 Cache ( in bytes )" << endl;
		cin >> l1;
		cout << "Enter the size of L2 Cache ( in bytes )" << endl;
		cin >> l2;
		cout << "Enter the Associativity " << endl;
		cin >> a;
		cout << "Enter the BlockSize ( in bytes )" << endl;
		cin >> b;
		cout << "Enter the time L1 cache takes to search and read/write a block in L1 cache" << endl;
		cin >> l1time;
		cout << "Enter the time L2 cache takes to search and read/write a block in L2 cache" << endl;
		cin >> l2time;
		cout << "Enter the time the WriteBuffer takes to search and read/write a block in WriteBuffer" << endl;
		cin >> buffertime;
		cout << "Enter the time the Main Memory takes to search and read/write a block in Main Memory" << endl;
		cin >> memorytime;
		cout << "Enter 1 to use LRU Replacement Policy, 2 to use LFU Replacement Policy, 3 to use FIFO Replacement Policy, 4 to use Random Replacement Policy, and 5 to LRU-LFU-FIFO Replacement Policy" << endl;
		cin >> replacementPolicy;
	}
	else if( option == '0' )
	{
		cacheEnabledOrNot = false;
	}
	else
	{
		cout << "Enter correct choice" << endl;
		goto cachee;
	}
Table:
	cout << "Enter 1 to print the Complete Execution table in the output file and 0 to not print the table" << endl;
	cout << "NOTE : printing the Execution Table to the output file takes longer time than expected" << endl;
	cin >> option;
	if( option == '1' )
		ext = true;
	else if( option == '0' )
		ext = false;
	else
	{
		cout << "Enter the correct choice" << endl;
		goto Table;
	}

	Cache* cache;
	int n = l1/b;
	int NoOfL1 = n/a;

	n = l2/b;
	int NoOfL2 = n/a;

	Set arr1[NoOfL1];
	Set arr2[NoOfL2];
	Cache c( l1 , l2 , b , a , arr1 , arr2 , replacementPolicy , l1time , l2time , memorytime , buffertime );

	if( cacheEnabledOrNot == true )
	{
		cache = &c;
	}
	else
	{
		cache = NULL;
	}

	Processor pr(cache);
	pr.Process( pipelineOrNot , operandForwardingOrNot , earlyBranchResolutionOrNot , stepWiseExecution , cacheEnabledOrNot , branchPredictorOrNot , ext );
	if( cacheEnabledOrNot == true )
	{
		cache->flushCache();
		cache->writebackbuffer();
	}
	reg.WriteToFile();
	mem.WriteToFile();
	pr.Print(-1,-1,-1);
}
