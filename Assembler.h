#include <bits/stdc++.h>
#include "Processor.h"
using namespace std;

map<string, pair<int,int>> reserveWords;
map<string, int> registersSet;
map<string, int> labels;

class Assembler
{
	int skipSpaces(string s, int idx)
	{
		while (idx<s.length() and  s[idx] == ' ')
		{
			idx++;
		}
		return idx;
	}

	int skipCommas(string s, int idx){
		while (idx<s.length() and  s[idx] == ','){
			idx++;
		}
		return idx;
	}

	int skipTabs(string s, int idx){
		while (idx<s.length() and  s[idx] == '\t'){
			idx++;
		}
		return idx;
	}

	int skiplines(string s, int idx){
		while (idx<s.length() and  s[idx] == '\n'){
			idx++;
		}
		return idx;
	}

	string getKeyword(string s, int &idx){
		string var = "";
		while (idx<s.length() and s[idx] != ' ' and s[idx]!='\n' and s[idx]!='\t' ){
			var += s[idx++];
		}
		return var;
	}

	public:

	int parse(string s, int idx){

		int ipc = 8200;
		int upc = 0000; 
		bool beforeMain = true;
		bool withAddress = true;
		bool getText = false;
		bool take_keyword = false;
		string main_;

		while(idx<s.length()){
			string initMain = "";
			while(idx<s.length() and beforeMain){
				if(s[idx]!=' ' or s[idx]!='\n' and beforeMain){
					idx = skipSpaces(s,idx);
					idx = skipTabs(s,idx);
					idx = skiplines(s,idx);
					idx = skipSpaces(s,idx);
					idx = skipTabs(s,idx);
					idx = skiplines(s,idx);
					initMain = getKeyword(s,idx);
					idx = skipSpaces(s,idx);
					idx = skipTabs(s,idx);
					idx = skiplines(s,idx);

					if(initMain.substr(0,1)=="#" || initMain=="#"){
						while(s[idx]!='\n'){
							idx++;
						}
						take_keyword = true;
					}

					if(take_keyword){
						idx = skipSpaces(s,idx);
						idx = skipTabs(s,idx);
						idx = skiplines(s,idx);
						idx = skipSpaces(s,idx);
						idx = skipTabs(s,idx);
						initMain = getKeyword(s,idx);
					}

					if(initMain == ".data"){
						initMain = "";
						int index = idx;
anotherValue:
						bool firstValue=true;
						idx = skipSpaces(s,idx);
						idx = skipTabs(s,idx);
						idx = skiplines(s,idx);
						idx = skipSpaces(s,idx);
						idx = skipTabs(s,idx);
						idx = skiplines(s,idx);
						initMain = getKeyword(s,idx);
						idx = skipSpaces(s,idx);
						idx = skipTabs(s,idx);
						idx = skiplines(s,idx);
						if (initMain != ".text"){
							string varName = initMain.substr(0,initMain.length()-1);
							if (initMain.back() != ':'){
								cout<<"variables not defined properly"<<endl;
								return -1;
							}else{
								initMain = "";
								idx = skipSpaces(s,idx);
								idx = skipTabs(s,idx);
								idx = skiplines(s,idx);
								initMain = getKeyword(s,idx);

								take_keyword = false;

								if(initMain.substr(0,1)=="#"){
									while(s[idx]!='.'){
										idx++;
									}
									take_keyword = true;
								}

								if(take_keyword){
									idx = skipSpaces(s,idx);
									idx = skipTabs(s,idx);
									idx = skiplines(s,idx);
									initMain = getKeyword(s,idx);
								}

								if(initMain==".word"){
									while(s[idx]!='\n'){
										idx = skipSpaces(s,idx);
										idx = skipTabs(s,idx);
										if(s[idx]==',')idx++;
										string num="";
										while(s[idx]!='\n' and s[idx]!=',' and s[idx]!=' '){
											num += s[idx];
											idx++;
										}
										if(num.substr(0,1)=="#"){
											while(s[idx]!='\n')idx++;
											goto anotherValue;
										}
										idx = skipCommas(s,idx);
										idx = skipSpaces(s,idx);
										idx = skipTabs(s,idx);
										int number = stoi(num);
										int numVal = number;
										if(firstValue){
											int opCode=number;
											number = number<<24;
											number = number|ipc;
											mem.writeWord(opCode,upc);
											firstValue=false;

											reserveWords.insert({varName,make_pair(numVal,upc)});
										}else{
											mem.writeWord(number,upc);
										}
										upc += 4;
									}
								}
							}
							goto anotherValue;
							idx = index;
						}
						else{
							getText = true;
						}
					}

					if(getText==false){
						idx = skipSpaces(s,idx);
						idx = skipTabs(s,idx);
						idx = skiplines(s,idx);
						idx = skipSpaces(s,idx);
						idx = skipTabs(s,idx);
						idx = skiplines(s,idx);
						initMain = getKeyword(s,idx);
						if (initMain == ".text"){
							initMain = "";
						}
					}

globl:

					idx = skipSpaces(s,idx);
					idx = skipTabs(s,idx);
					idx = skiplines(s,idx);
					idx = skipSpaces(s,idx);
					idx = skipTabs(s,idx);
					idx = skiplines(s,idx);
					initMain = getKeyword(s,idx);

					take_keyword = false;

					if(initMain.substr(0,1)=="#"){
						while(s[idx]!='.'){
							idx++;
						}
						take_keyword = true;
					}

					if(take_keyword){
						idx = skipSpaces(s,idx);
						idx = skipTabs(s,idx);
						idx = skiplines(s,idx);
						initMain = getKeyword(s,idx);
					}

					if(initMain == ".globl"){
						initMain = "";
					}
					else
					{
						cout << ".globl is missing" << endl;
						return -1;
					}

					idx = skipSpaces(s,idx);
					idx = skipTabs(s,idx);
					idx = skiplines(s,idx);
					idx = skipSpaces(s,idx);
					idx = skipTabs(s,idx);
					idx = skiplines(s,idx);
					initMain = getKeyword(s,idx);

					main_ = initMain;
					initMain = "";
					beforeMain = false;
				}
			}
			int ipc2 = ipc;
			if(!beforeMain){  
				int index = idx;     
				string instruction = "";
				for (int x = 1; x <=2; x++){   
					if( x == 2 )
					{	
						map<string,int>::iterator itr;
						for( itr = labels.begin() ; itr != labels.end() ; itr++ )
						{
							cout << itr->first << "\t" << itr->second << endl;
						}
					}
					ipc = ipc2;
					idx = index;
newInst:
					idx = skipSpaces(s,idx);
					idx = skipTabs(s,idx);
					idx = skiplines(s,idx);

					if(idx>=s.length() and x==1){
						ipc = ipc2;
						idx = index;
						x=2;
					}


					idx = skipSpaces(s,idx);
					idx = skipTabs(s,idx);
					idx = skiplines(s,idx);
					idx = skipSpaces(s,idx);
					idx = skipTabs(s,idx);
					idx = skiplines(s,idx);
					instruction = getKeyword(s,idx);

					if(instruction.substr(0,1)=="#"){
						while(s[idx]!='\n'){
							idx++;
						}
						goto newInst;
					}


					int registerInst[3] = {-1};

					if (instruction == "add" || instruction == "sub"  || instruction=="and"  || instruction=="or" || instruction=="mul"){
						int registerIdx = 0;
						while (registerIdx <= 2){
							idx = skipSpaces(s,idx);
							idx = skipTabs(s,idx);
							if (s[idx] != '$'){
								cout<<"Error : expecting $ in line "<<(ipc-8196)/4<<endl;
								return -1;
							}
							else{
								idx++;
								string reg = "";
								reg += s[idx];
								if(reg=="z"){
									reg += s[idx + 1];
									reg += s[idx + 2];
									reg += s[idx + 3]; 
								}else reg += s[idx + 1];

								if (registersSet.find(reg) == registersSet.end()){
									cout<<"Error : Register not found in line "<<(ipc-8196)/4<<endl;
									return -3;
								}else{
									auto it = registersSet.find(reg);
									registerInst[registerIdx++] = it->second;
									if(reg=="zero")idx +=4;
									else idx += 2;
								}
								idx = skipSpaces(s,idx);
								idx = skipTabs(s,idx);
								if (s[idx] != ',' and registerIdx != 3){
									cout<<"Error : expecting a comma in line "<<(ipc-8196)/4<<endl;
									return -4;
								}
								idx = skipCommas(s,idx);
								idx = skipSpaces(s,idx);
							}
						}


						if (x==2 and instruction=="add"){   
							int opCode = 1;
							opCode = opCode << 8;
							int reg1 = registerInst[0];
							opCode = opCode | reg1;
							opCode = opCode << 8;
							int reg2 = registerInst[1];
							opCode = opCode | reg2;
							opCode = opCode << 8;
							int reg3 = registerInst[2];
							opCode = opCode | reg3;

							mem.writeWord(opCode,ipc);
						}
						if(instruction=="add")
							ipc += 4;

						if (x==2 and instruction=="sub"){   
							int opCode = 2;
							opCode = opCode << 8;
							int reg1 = registerInst[0];
							opCode = opCode | reg1;
							opCode = opCode << 8;
							int reg2 = registerInst[1];
							opCode = opCode | reg2;
							opCode = opCode << 8;
							int reg3 = registerInst[2];
							opCode = opCode | reg3;

							mem.writeWord(opCode,ipc);
						}

						if(instruction=="sub")
							ipc += 4;

						if (x==2 and instruction=="and"){   
							int opCode = 13;
							opCode = opCode << 8;
							int reg1 = registerInst[0];
							opCode = opCode | reg1;
							opCode = opCode << 8;
							int reg2 = registerInst[1];
							opCode = opCode | reg2;
							opCode = opCode << 8;
							int reg3 = registerInst[2];
							opCode = opCode | reg3;

							mem.writeWord(opCode,ipc);
						}
						if(instruction=="and")
							ipc += 4;

						if (x==2 and instruction=="or"){   
							int opCode = 14;
							opCode = opCode << 8;
							int reg1 = registerInst[0];
							opCode = opCode | reg1;
							opCode = opCode << 8;
							int reg2 = registerInst[1];
							opCode = opCode | reg2;
							opCode = opCode << 8;
							int reg3 = registerInst[2];
							opCode = opCode | reg3;

							mem.writeWord(opCode,ipc);
						}
						if(instruction=="or")
							ipc += 4;

						if (x==2 and instruction=="mul"){   
							int opCode = 12;
							opCode = opCode << 8;
							int reg1 = registerInst[0];
							opCode = opCode | reg1;
							opCode = opCode << 8;
							int reg2 = registerInst[1];
							opCode = opCode | reg2;
							opCode = opCode << 8;
							int reg3 = registerInst[2];
							opCode = opCode | reg3;

							mem.writeWord(opCode,ipc);
						}
						if(instruction=="mul")
							ipc += 4;

						goto newInst;
					}

					if(instruction=="bne" || instruction=="beq" || instruction=="blt" || instruction=="bgt"){
						int registerIdx = 0;
						while (registerIdx <= 1){
							idx = skipSpaces(s,idx);
							idx = skipTabs(s,idx);
							if (s[idx] != '$'){
								cout<<"Error : expecting a $ in line "<<(ipc-8196)/4<<endl;
								return -1;
							}else{
								idx++;
								string reg = "";
								reg += s[idx];
								if(reg=="z"){
									reg += s[idx + 1];
									reg += s[idx + 2];
									reg += s[idx + 3]; 
								}else reg += s[idx + 1];

								if (registersSet.find(reg) == registersSet.end()){
									cout<<"Error : Register not found in line "<<(ipc-8196)/4<<endl;
									return -3;
								}else{
									auto it = registersSet.find(reg);
									registerInst[registerIdx++] = it->second;
									if(reg=="zero")idx +=4;
									else idx += 2;
								}
								idx = skipSpaces(s,idx);
								idx = skipTabs(s,idx);
								if (s[idx] != ',' and registerIdx != 2){
									cout<<"Error : expecting a $ in line "<<(ipc-8196)/4<<endl;
									return -4;
								}else{
									idx++;
								}
							}
						}
						idx = skipSpaces(s,idx);
						string label = "";
						idx = skipSpaces(s,idx);
						idx = skipTabs(s,idx);
						label = getKeyword(s,idx);
						int lbl;

						if(x==2){
							if (labels.find(label)==labels.end()){
								cout<<"Error : label not found in line "<<(ipc-8196)/4<<endl;
								return -1;
							}else{
								lbl = labels.find(label)->second;
							}
						}

						if(x==2 and instruction=="bne"){
							int opCode = 6;
							opCode = opCode << 5;
							int reg1 = registerInst[0];
							opCode = opCode | reg1;
							opCode = opCode << 5;
							int reg2 = registerInst[1];
							opCode = opCode | reg2;
							opCode = opCode << 14;
							opCode = opCode | lbl;

							mem.writeWord(opCode,ipc);
						}
						if(instruction=="bne")
							ipc += 4;

						if(x==2 and instruction=="beq"){
							int opCode = 5;
							opCode = opCode << 5;
							int reg1 = registerInst[0];
							opCode = opCode | reg1;
							opCode = opCode << 5;
							int reg2 = registerInst[1];
							opCode = opCode | reg2;
							opCode = opCode << 14;
							opCode = opCode | lbl;

							mem.writeWord(opCode,ipc);
						}
						if(instruction=="beq")
							ipc += 4;

						if(x==2 and instruction=="blt"){
							int opCode = 10;
							opCode = opCode << 5;
							int reg1 = registerInst[0];
							opCode = opCode | reg1;
							opCode = opCode << 5;
							int reg2 = registerInst[1];
							opCode = opCode | reg2;
							opCode = opCode << 14;
							opCode = opCode | lbl;

							mem.writeWord(opCode,ipc);
						}

						if(instruction=="blt")
							ipc += 4;

						if(x==2 and instruction=="bgt"){
							int opCode = 15;
							opCode = opCode << 5;
							int reg1 = registerInst[0];
							opCode = opCode | reg1;
							opCode = opCode << 5;
							int reg2 = registerInst[1];
							opCode = opCode | reg2;
							opCode = opCode << 14;
							opCode = opCode | lbl;

							mem.writeWord(opCode,ipc);
						}

						if(instruction=="bgt")
							ipc += 4;

						goto newInst;
					}

					bool withAddress = true;
					if (instruction == "lw" || instruction == "sw"){   
						string temp = s;
						for(int index=idx;temp[index]!='\n' and index<s.length();index++){
							if(temp[index]=='('){
								withAddress = false;
								break;
							}
						}
						int registerIdx = 0;
						while (registerIdx <= 0){
							idx = skipSpaces(s,idx);
							idx = skipTabs(s,idx);
							if (s[idx] != '$'){
								cout<<"Error : expecting a $ in line "<<(ipc-8196)/4<<endl;
								return -1;
							}else{
								idx++;
								string reg = "";
								reg += s[idx];
								if(reg=="z"){
									reg += s[idx + 1];
									reg += s[idx + 2];
									reg += s[idx + 3]; 
								}else reg += s[idx + 1];

								if (registersSet.find(reg) == registersSet.end()){
									cout<<"Error : Register not found in line "<<(ipc-8196)/4<<endl;
									return -3;
								}else{
									auto it = registersSet.find(reg);
									registerInst[registerIdx++] = it->second;
									if(reg=="zero")idx +=4;
									else idx += 2;
								}
								idx = skipSpaces(s,idx);
								idx = skipTabs(s,idx);
								if (s[idx] != ','){
									cout<<"Error : expecting a $ in line "<<(ipc-8196)/4<<endl;
									return -1;
								}else{
									idx++;
								}
								idx = skipSpaces(s,idx);
								idx = skipTabs(s,idx);
							}
						}
						idx = skipSpaces(s,idx);

						if(withAddress==false){
							int offset = 0;
							while (s[idx] >= '0' and s[idx] <= '9'){
								offset = offset * 10 + s[idx]-'0';
								idx++;
							}
							idx = skipSpaces(s,idx);
							idx = skipTabs(s,idx);
							if (s[idx] != '('){
								cout<<"Error : expecting ( in line "<<(ipc-8196)/4<<endl;
								return -1;
							}else{
								idx++;
							}
							idx = skipSpaces(s,idx);
							idx = skipTabs(s,idx);
							if (s[idx] != '$'){
								cout<<"Error : expecting a $ in line "<<(ipc-8196)/4<<endl;
								return -1;
							}else{
								idx++;
							}
							string reg = "";
							reg += s[idx];
							if(reg=="z"){
								reg += s[idx + 1];
								reg += s[idx + 2];
								reg += s[idx + 3]; 
							}else reg += s[idx + 1];

							if (registersSet.find(reg) == registersSet.end()){
								cout<<"Error : Register not found in line "<<(ipc-8196)/4<<endl;
								return -3;
							}else{
								auto it = registersSet.find(reg);
								registerInst[registerIdx++] = it->second;
								if(reg=="zero")idx +=4;
								else idx += 2;
							}
							idx = skipSpaces(s,idx);
							idx = skipTabs(s,idx);
							if (s[idx] != ')'){
								cout<<"Error : expecting ) in line "<<(ipc-8196)/4<<endl;
								return -1;
							}else{
								idx++;
							}

							if (x==2 and instruction=="lw"){
								int opCode = 3;
								opCode = opCode << 8;
								int reg1 = registerInst[0];
								opCode = opCode | reg1;
								opCode = opCode << 8;
								int ofst = offset;
								opCode = opCode | ofst;
								opCode = opCode << 8;
								int reg2 = registerInst[1];
								opCode = opCode | reg2;

								mem.writeWord(opCode,ipc);
							}
							if(instruction=="lw")
								ipc += 4;

							if (x==2 and instruction=="sw"){
								int opCode = 4;
								opCode = opCode << 8;
								int reg1 = registerInst[0];
								opCode = opCode | reg1;
								opCode = opCode << 8;
								int ofst = offset;
								opCode = opCode | ofst;
								opCode = opCode << 8;
								int reg2 = registerInst[1];
								opCode = opCode | reg2;

								mem.writeWord(opCode,ipc);

							}
							if(instruction=="sw")
								ipc += 4;
						}else{
							string var = getKeyword(s,idx);
							if(reserveWords.find(var)==reserveWords.end()){
								cout<<"Error : variable not found in line "<<(ipc-8196)/4<<endl;
								return -1;
							}else{
								if(x==2 and instruction=="lw"){
									int opCode = 3;
									opCode = opCode << 1;
									opCode = opCode|1;
									opCode = opCode<<7;
									int reg1 = registerInst[0];
									opCode = opCode | reg1;
									opCode = opCode << 16;
									int wordAdd = reserveWords.find(var)->second.second;
									opCode = opCode | wordAdd;

									mem.writeWord(opCode,ipc);
								}
								if(instruction=="lw")
									ipc += 4;
							}
						}

						goto newInst;
					}

					if (instruction == "li"){
						int registerIdx = 0;
						while (registerIdx <= 0){
							idx = skipSpaces(s,idx);
							idx = skipTabs(s,idx);
							if (s[idx] != '$'){
								cout<<"Error : expecting a $ in line "<<(ipc-8196)/4<<endl;
								return -1;
							}
							else{
								idx++;
								string reg = "";
								reg += s[idx];
								if(reg=="z"){
									reg += s[idx + 1];
									reg += s[idx + 2];
									reg += s[idx + 3]; 
								}else reg += s[idx + 1];

								if (registersSet.find(reg) == registersSet.end()){
									cout<<"Error : Register not found in line "<<(ipc-8196)/4<<endl;
									return -3;
								}else{
									auto it = registersSet.find(reg);
									registerInst[registerIdx++] = it->second;
									if(reg=="zero")idx +=4;
									else idx += 2;
								}
								idx = skipSpaces(s,idx);
								idx = skipTabs(s,idx);
								if (s[idx] != ',' and registerIdx != 1){
									cout<<"Error : expecting a comma in line "<<(ipc-8196)/4<<endl;
									return -1;
								}else{
									idx++;
								}
							}
						}
						idx = skipSpaces(s,idx);
						string num = "";
						while(s[idx]!='\n' and s[idx]!=' ' and s[idx] != '\t' ){
							num += s[idx++];
						}
						int number = stoi(num);

						if (x==2 and instruction=="li"){   
							int opCode = 8;
							opCode = opCode << 8;
							int reg1 = registerInst[0];
							opCode = opCode | reg1;
							opCode = opCode << 16;
							opCode = opCode | number;

							mem.writeWord(opCode,ipc);
						}
						ipc += 4;

						goto newInst;
					}

					if(instruction=="j"){
						string label = "";
						idx = skipSpaces(s,idx);
						idx = skipTabs(s,idx);
						label = getKeyword(s,idx);
						int lbl;

						if(x==2){
							if (labels.find(label)==labels.end()){
								cout<<"Error : label not found in line "<<(ipc-8196)/4<<endl;
								return -1;
							}else{
								lbl = labels.find(label)->second;
							}
						}

						if(x==2 and instruction=="j"){
							int opCode = 7;
							opCode = opCode << 24;
							opCode = opCode | lbl;
							mem.writeWord(opCode,ipc);
						}
						ipc += 4;

						goto newInst;
					}

					if(instruction=="la"){
						int registerIdx = 0;
						while (registerIdx <= 0){
							idx = skipSpaces(s,idx);
							idx = skipTabs(s,idx);
							if (s[idx] != '$'){
								cout<<"Error : expecting a $ in line "<<(ipc-8196)/4<<endl;
								return -1;
							}else{
								idx++;
								idx = skipSpaces(s,idx);
								string reg = "";
								reg += s[idx];
								if(reg=="z"){
									reg += s[idx + 1];
									reg += s[idx + 2];
									reg += s[idx + 3]; 
								}else reg += s[idx + 1];

								if (registersSet.find(reg) == registersSet.end()){
									cout<<"Error : Register not found in line "<<(ipc-8196)/4<<endl;
									return -3;
								}else{
									auto it = registersSet.find(reg);
									registerInst[registerIdx++] = it->second;
									if(reg=="zero")idx +=4;
									else idx += 2;
								}
								idx = skipSpaces(s,idx);
								idx = skipTabs(s,idx);
								if (s[idx] != ',' and registerIdx != 1){
									cout<<"Error : expecting a comma in line "<<(ipc-8196)/4<<endl;
									return -4;
								}else{
									idx++;
								}
							}
						}

						string label = "";
						idx = skipSpaces(s,idx);
						idx = skipTabs(s,idx);
						label = getKeyword(s,idx);
						int lbl;
						if(x==2){
							if (reserveWords.find(label)==reserveWords.end()){
								if( labels.find(label) == labels.end() )
								{
									cout<<"Error : variable not found in line "<<(ipc-8196)/4<<endl;
									return -1;
								}
								else
								{
									lbl = labels.find(label)->second;
								}
							}else{
								lbl = reserveWords.find(label)->second.second;
							}
						}

						if (x==2 and instruction=="la"){   
							int opCode = 9;
							opCode = opCode << 8;
							int reg1 = registerInst[0];
							opCode = opCode | reg1;
							opCode = opCode << 16;
							opCode = opCode | lbl;

							mem.writeWord(opCode,ipc);
						}
						ipc += 4;

						goto newInst;

					}

					if(instruction=="addi"){
						int registerIdx = 0;
						while (registerIdx <= 1){
							idx = skipSpaces(s,idx);
							idx = skipTabs(s,idx);
							if (s[idx] != '$'){
								cout<<"Error : expecting $ in line "<<(ipc-8196)/4<<endl;
								return -1;
							}
							else{
								idx++;
								string reg = "";
								reg += s[idx];
								if(reg=="z"){
									reg += s[idx + 1];
									reg += s[idx + 2];
									reg += s[idx + 3]; 
								}else reg += s[idx + 1];

								if (registersSet.find(reg) == registersSet.end()){
									cout<<"Error : Register not found in line "<<(ipc-8196)/4<<endl;
									return -3;
								}else{
									auto it = registersSet.find(reg);
									registerInst[registerIdx++] = it->second;
									if(reg=="zero")idx +=4;
									else idx += 2;
								}
								idx = skipSpaces(s,idx);
								idx = skipTabs(s,idx);
								if (s[idx] != ',' and registerIdx != 2){
									cout<<"Error : expecting a comma in line "<<(ipc-8196)/4<<endl;
									return -4;
								}
								idx = skipCommas(s,idx);
								idx = skipSpaces(s,idx);
							}
						}

						idx = skipSpaces(s,idx);
						idx = skipTabs(s,idx);

						string number = "";
						while(s[idx]>='0' and s[idx]<='9'){
							number += s[idx++];
						}
						int no_ = stoi(number);

						if(x==2 and instruction=="addi"){
							int opCode = 11;
							opCode = opCode << 5;
							int reg1 = registerInst[0];
							opCode = opCode | reg1;
							opCode = opCode << 5;
							int reg2 = registerInst[1];
							opCode = opCode | reg2;
							opCode = opCode << 14;
							opCode = opCode | no_;

							mem.writeWord(opCode,ipc);
						}

						if(instruction=="addi")
							ipc += 4;

						goto newInst;
					}

					if(instruction=="jr"){
						int registerIdx = 0;
						while (registerIdx <= 0){
							idx = skipSpaces(s,idx);
							idx = skipTabs(s,idx);
							if (s[idx] != '$'){
								cout<<"Error : expecting $ in line "<<(ipc-8196)/4<<endl;
								return -1;
							}
							else{
								idx++;
								string reg = "";
								reg += s[idx];
								if(reg=="z"){
									reg += s[idx + 1];
									reg += s[idx + 2];
									reg += s[idx + 3]; 
								}else reg += s[idx + 1];

								if (registersSet.find(reg) == registersSet.end()){
									cout<<"Error : Register not found in line "<<(ipc-8196)/4<<endl;
									return -3;
								}else{
									auto it = registersSet.find(reg);
									registerInst[registerIdx++] = it->second;
									if(reg=="zero")idx +=4;
									else idx += 2;
								}
								idx = skipSpaces(s,idx);
								idx = skipTabs(s,idx);
								if (s[idx] != ',' and registerIdx != 1){
									cout<<"Error : expecting a comma in line "<<(ipc-8196)/4<<endl;
									return -4;
								}else{
									idx++;
								}
								idx = skipSpaces(s,idx);
							}
						}

						idx = skipSpaces(s,idx);
						idx = skipTabs(s,idx);

						if(x==2 and instruction=="jr"){
							int opCode = 16;
							opCode = opCode << 24;
							int reg1 = registerInst[0];
							opCode = opCode | reg1;

							mem.writeWord(opCode,ipc);
						}

						if(instruction=="jr")
							ipc += 4;

						goto newInst;
					}


					if(instruction=="jal"){
						string label = "";
						idx = skipSpaces(s,idx);
						idx = skipTabs(s,idx);
						label = getKeyword(s,idx);
						int lbl;

						if(x==2){
							if (labels.find(label)==labels.end()){
								cout<<"Error : label not found in line "<<(ipc-8196)/4<<endl;
								return -1;
							}else{
								lbl = labels.find(label)->second;
							}
						}

						if (x==2 and instruction=="jal"){   
							int opCode = 17;
							opCode = opCode << 24;
							opCode = opCode | lbl;

							mem.writeWord(opCode,ipc);
						}
						ipc += 4;

						goto newInst;

					}

					if(x==1){   
						if( instruction.at( instruction.length()-1 ) != ':' )
						{
							cout << "Unknown string encountered" << endl;
							return -1;
						}
						instruction = instruction.substr(0,instruction.length()-1);
						labels.insert(make_pair(instruction, ipc));
						goto newInst;
					}
				}
			}

		}
		int lbl;
		if( labels.find(main_) == labels.end() )
		{
			cout << "Main Not Found" << endl;
			return -1;
		}
		else
			lbl = labels.find(main_)->second;
		int n = 17;
		n = n << 24;
		n = n | lbl;
		mem.writeWord( n , 8192 );
		return 1;
	}
};
