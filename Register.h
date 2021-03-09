#include <bits/stdc++.h>
using namespace std;

class Pair{
    public:
        int value;
        bool isUsed;
};

class Register{

    Pair registers[32] = {0};
    string registerName[1000] = {"zero","at","v0","v1","a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "s8", "ra"};

    public:
        void initializeRegisters(){
            for(int i=0;i<32;i++){
                registers[i].value=0;
                registers[i].isUsed=false;
            }
        }

        void showAllRegisters(){
            cout<<endl<<"Printing the registers"<<endl;
            for(int i=0;i<32;i++){
                cout<<registerName[i]<<" ---> "<<registers[i].value<<endl;
            }
        } 

        int getRegister(int position){
            return registers[position].value;
        }

        void setRegister(int position, int value){
            registers[position].value = value;
        }

        void setRegisterIsUsedOrNot(int position, bool used){
            registers[position].isUsed = used;
        }

        bool getRegisterIsUsedOrNot(int position){
            return registers[position].isUsed;
        }

};
