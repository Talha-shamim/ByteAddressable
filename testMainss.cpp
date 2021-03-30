#include"parser.h"
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
    if(f){
        ostringstream ss;
        ss << f.rdbuf();
        str = ss.str();
    }
    else{
	    cout << "The file doesn't exist or cannot be opened" << endl;
	    return 0;
    }

    bool stp = false;
    bool pip = false;
    bool opf = false;

    label:

    cout<<"Enter yes for stepwise execution"<<endl;
    string stw;
    cin>>stw;
    if(stw=="yes"){
        stp = true;
    }
    else if(stw == "no"){
        cout<<"Enter yes for pipeline execution"<<endl;
        cin>>stw;
        if(stw=="yes"){
            pip = true;
        }

        cout<<"Enter yes for operand forward execution"<<endl;
        cin>>stw;
        if(stw=="yes"){
            opf = true;
        }
    }else{
        cout<<"Enter correct value"<<endl;
        goto label;
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

    Parser parser;

    cout << "Memory and Registers before Parsing and storing the instructions" << endl;
    mem.print();
    reg.showAllRegisters();
    int i = parser.parse( str , 0 );
    if( i != 1 ){
	    cout << "Error while Parsing the program" << endl;
	    cout << "Terminating...." << endl;
	    return 0;
    }
    cout << "Memory and Registers after parsing and before processing" << endl;
    mem.print();
    reg.showAllRegisters();
    Processor pr;
    pr.Process(pip,opf,false,stp);
    cout << "Memory and Registers after successfully Executing the program" << endl;
    mem.print();
    reg.showAllRegisters();

    reg.WriteToFile();
    mem.WriteToFile();  

    return 0;
}