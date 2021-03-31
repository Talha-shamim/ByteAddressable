#include"Assembler.h"
#include<iostream>
#include<bits/stdc++.h>
#include<fstream>

int main(){

	cout << "Enter the file name along with the extension( .s or .asm or .txt )" << endl;
	cout << "( the file should be in the same directory that contains this code )" << endl;
	string filename;
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
		cout << "The file doesn't exist or cannot be opened" << endl;
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

	
	Assembler assemble;
	int i = assemble.parse( str , 0 );
	if( i != 1 ){
		cout << "Error while Parsing the program" << endl;
		cout << "Terminating...." << endl;
		return 0;
	}
	cout << "Memory and Registers after parsing and before processing" << endl;

	Processor pr;
	int n = 7;
	n = n << 24;
	n = n | 8200;
	mem.writeWord( n , 8192 );
	mem.WriteInstructions();
	pr.Print(-1, -1, -1);


	bool stp = false;
	bool pip = false;
	bool opf = false;
label:
	
	cout<<"Enter yes for stepwise execution, no for Normal Execution"<<endl;
	string stw;
	cin>>stw;
	if(stw=="yes")
	{
		stp = true;
	}
	else if(stw == "no")
	{
		cout<<"Enter yes for pipeline execution, no for Unpipelined Execution"<<endl;
		cin>>stw;
		if(stw=="yes")
		{
			pip = true;
			cout<<"Enter yes to Enable Operand Forwarding, no to Disable Operand Forwarding"<<endl;
			cin>>stw;
			if(stw=="yes")
			{
				opf = true;
			}
			else if( stw == "no" )
			{
				opf = false;
			}
			else
			{
				cout << "Enter Correct Value, Try Again" << endl;
				goto label;
			}
		}
		else if( stw == "no" )
		{
			pip = false;
		}
		else
		{
			cout << "Enter Correct Value, Try Again" << endl;
			goto label;
		}
	}
	else
	{
        	cout<<"Enter correct value"<<endl;
        	goto label;
	}

	string output = pr.Process(pip,opf,false,stp);

	reg.initializeRegisters();

	Assembler parser1;
	parser1.parse( str , 0 );
	Processor pr1;
	pr1.Process( false , false , false , false );
	reg.initializeRegisters();

	Assembler parser2;
	parser2.parse( str , 0 );
	Processor pr2;
	pr2.Process( true , false , false , false );
	reg.initializeRegisters();

	Assembler parser3;
	parser3.parse( str , 0 );
	Processor pr3;
	pr3.Process( true , true , false , false );
	
	if( output != "" )
	{
		cout << "Program Executed Successfully" << endl;
		cout << output << endl;
	}
	else
		return 0;
	pr.Print( -1, -1, -1 );  
	cout << "\e[36m" << "\nYou can take a look at the Entire Memory , and Registers by navigating to the files- "<< "\e[35m" << "\nUserMemory.txt, InstructionMemory.txt and Registers.txt\n" << endl;
	cout << "\e[36m" << "You can also view the Complete Statistics and also the Clock Cycle table of all the three types of Executions( Unpipeline, pipeline with Operand forwarding , \npipeline without Operand forwarding ) by navigating to the below files( located in this same directory ):" << endl;
	cout << "\e[35m" << " Unpipelined.txt , PipelineWithOperandForwarding.txt , PipelineWithoutOperandForwarding.txt \n" << endl;
	return 0;
}
