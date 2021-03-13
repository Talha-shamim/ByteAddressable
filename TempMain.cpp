#include"assembler.h"
#include<iostream>
#include<fstream>
int main()
{
    ifstream f("bubbleSort.txt");
    string str;
    if(f){
        ostringstream ss;
        ss << f.rdbuf();
        str = ss.str();
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

    Assembler parser;

    mem.print();
    reg.showAllRegisters();
    cout << parser.parse(str, 0) << endl;
    mem.print();
    Processor pr;
    pr.Process();
    mem.print();
    reg.showAllRegisters();

    return 0;
}
