#include <bits/stdc++.h>
using namespace std;

// claa pair with atributes value and isUsed
class Pair{
    public:
        int value;
        bool isUsed;
};

class Register{

    // initializing the registers name and register array
    Pair registers[32] = {0};
	public:
    string registerName[1000] = {"zero","at","v0","v1","a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "s8", "ra"};

        // to set all registers value to 0 and isUsed to false
        void initializeRegisters(){
            for(int i=0;i<32;i++){
                registers[i].value=0;
                registers[i].isUsed=false;
            }
        }

        // for showing all ther registers
        void showAllRegisters(){
            cout<<endl<<"\e[35m" <<"                  || Printing the registers ||"<<"\e[0m" <<endl;
            for(int i=0;i<32;i++){
                cout<<"|----------------------------------------------------------------------|"<<endl;
                cout<<"                     "<<registerName[i]<<"       --->        "<<registers[i].value<<endl;
            }
        } 

        // for returning the content of 1 registers with name and value
        string getOneRegister(int r_no){
            string str = "";
            for(int i=0;i<32;i++){
                if(i==r_no){
                    str+= registerName[i];
                    str+= "    -->   ";
                    str+= registers[i].value;
                }
            }
            return str;
        }

        // to display the registers in a go
        void showStepWiseRegisters(int r_no){
            
            cout<<endl<<"\e[35m" <<"                  || Printing the registers ||"<<"\e[0m" <<endl;
            for(int i=0;i<32;i++){
                if(i==r_no){
                cout<<"|------------------------------------------------------------------------|"<<endl;
                    cout<<"          "<< "\e[33m" <<registerName[i]<<  "\e[0m" << "       --->        " << "\e[33m" <<registers[i].value<< "\e[0m" << endl;
                }else{
                cout<<"|-----------------------------------------------------------------------------|"<<endl;
                cout<<"                     "<<registerName[i]<<"       --->        "<<registers[i].value<<endl;
                }
            }
            
        } 

        // for setting the attributr value
        int getRegister(int position){
            return registers[position].value;
        }

        // for returning the value of a particular register
        void setRegister(int position, int value){
            registers[position].value = value;
        }

        // for setting the attribiute isUed
        void setRegisterIsUsedOrNot(int position, bool used){
            registers[position].isUsed = used;
        }

        // for returning is the register is used or not
        bool getRegisterIsUsedOrNot(int position){
            return registers[position].isUsed;
        }

        // for writing the content of registers to a file
        void WriteToFile(){
			string str = "";
			str = str + "    Registers\n";
			str = str + "Num\t" + "Name\t" + "Value\n";
			for( int i = 0 ; i < 32 ; i++ ){
				str = str + "$" + to_string(i) + "\t$" + registerName[i] + "\t" + to_string(getRegister(i)) + "\n";
			}
			ofstream out("Registers.txt");
			out << str;
			out.close();
		}
};
