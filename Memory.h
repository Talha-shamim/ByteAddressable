#include<iostream>
#include<fstream>
#include<sstream>

using namespace std;
string Instructions[1000];
int size = 0;

class Memory
{
	private:
		unsigned char mem[16384] = {0};
		
	public:
		int getWord( int address )
		{
			int j = 0;
			j = j | mem[address];
			j = j << 8;
			j = j | mem[address+1];
			j = j << 8;
			j = j | mem[address+2];
			j = j << 8;
			j = j | mem[address+3];

			return j;
		}

		void writeWord( int word , int address )
		{
			mem[address+3] = 0;
			mem[address+2] = 0;
			mem[address+1] = 0;
			mem[address]  = 0;
			mem[address+3] = mem[address+3] | word;
			word = word >> 8;
			mem[address+2] = mem[address+2] | word;
			word = word >> 8;
			mem[address+1] = mem[address+1] | word;
			word = word >> 8;
			mem[address] = mem[address] | word;
		}

		unsigned char getByte( int address )
		{
			return mem[address];
		}

		void writeByte( unsigned char c , int address )
		{
			mem[address] = c;
		}

		void print()
		{
			cout << "User Memory" << endl;
			for( int i = 0 ; i < 150 ; i+=4 )
			{
				int k = getWord( i );
				int n = 1 << 31;
				cout << " " << i << "   ";
				
				for( int j = 1 ; j <= 32 ; j++ )
				{
					if( n & k )
						cout << "1 ";
					else
						cout << "* ";
					if( j % 8 == 0 )
						cout << "  ";

					k = k << 1;
				}
				cout << "  ";
				cout << getWord(i) << endl;
			}

			cout << "Instruction Memory" << endl;
			for( int i = 8192 ; i < 8400 ; i+=4 )
			{
				int k = getWord( i );
				int n = 1 << 31;
				cout << " " << i << "   "; 
				
				for( int j = 1 ; j <= 32 ; j++ )
				{
					if( n & k )
						cout << "1 ";
					else
						cout << "* ";
					if( j % 8 == 0 )
						cout << "  ";

					k = k << 1;
				}
				cout << "  ";
				cout << getWord( i ) << endl;
			}
		}

		void WriteToFile()
		{
			string str = "                            User Memory( WordWise )\n";
			str = str + "Address                 Binary Representation of the value                        Value\n";
			int k;
			string s;
			for( int i = 0 ; i < 8192 ; i += 4 )
			{
				k = getWord( i );
				stringstream ss;
				ss << hex << i;
				s = ss.str();
				str = str + "0x" + s;
				str = str + "\t";
				
				int n = 1 << 31;
				for( int j = 1 ; j <= 32 ; j++ )
				{
					if( n & k )
						str = str + "1 ";
					else
						str = str + "* ";
					if( j % 8 == 0 )
						str = str + "  ";

					k = k << 1;
				}
				
				str = str + "    ";
				str = str + to_string(getWord(i)) + "\n";
			}
			ofstream out1("UserMemory.txt");
			out1 << str;
			out1.close();
			
			str = "                            Instruction Memory( WordWise )\n";
			str = str + "Address                  Binary Representation of the Instruction                 Instruction\n";
			for( int i = 8192 ; i < 16383 ; i += 4 )
			{
				k = getWord( i );
				
				str = str + to_string(i);
				str = str + "\t";

				int n = 1 << 31;
				for( int j = 1 ; j <= 32 ; j++ )
				{
					if( n & k )
						str = str + "1 ";
					else
						str = str + "* ";
					if( j % 8 == 0 )
						str = str + " ";
					k = k << 1;
				}

				str = str + "    ";
				str = str + GetInstruction( getWord(i) ) + "\n"; 
			}
			ofstream out2("InstructionMemory.txt");
			out2 << str;
			out2.close();
		}
		void WriteInstructions()
		{
			for( int i = 8200 ; i < 16383 ; i += 4 )
			{
				if( getWord(i) != 0 )
					size++;
				else
					break;
			}

			int pc = 8200;
			
			for( int i = 1 ; i <= size ; i++ )
			{
				Instructions[i] = GetInstruction( getWord(pc) );
				pc = pc + 4;
			}
		}

		string GetInstruction( int inst )
		{
			string ins = "";
			int c = 0;
			int n = 255;
			n = n << 24;
			c = inst & n;
			c = c >> 24;

			if( c == 1 )
			{
				int n = 255;
				int ad2 = n & inst;
				
					
				n = n << 8;
				int ad1 = n & inst;
				ad1 = ad1 >> 8;

				n = n << 8;
				int add = n & inst;
				add = add >> 16;

				ins = ins + "add " + "$" + to_string(add) + ",$" + to_string(ad1) + ",$" + to_string(ad2);

			}
			else if( c == 2 )
			{
				int n = 255;
				int ad2 = n & inst;

				n = n << 8;
				int ad1 = n & inst;
				ad1 = ad1 >> 8;
					
				n = n << 8;
			       	int add = n & inst;
				add = add >> 16;

				ins = ins + "sub " + "$" + to_string(add) + ",$" + to_string(ad1) + ",$" + to_string(ad2);
			}
			else if( c == 3 )
			{
				int z = 1;
			       	z = z << 23;
				int y = z & inst;
				y = y >> 23;
				if( y == 1 )
				{
					int n = 16383;
					int value1 = n & inst;
					n = 31;
					n = n << 16;
					int ad = n & inst;
					ad = ad >> 16;
					ins = ins + "lw " + "$" + to_string(ad) + "," + to_string(value1) + "\t";
					}
				else
				{
					int n = 255;
					int ad2 = n & inst;
										
					n = n << 8;
					int ad1 = n & inst;
					ad1 = ad1 >> 8;
	
					n = n << 8;
					int add = n & inst;
					add = add >> 16;

					ins = ins + "lw " + "$" + to_string(add) + "," + to_string(ad1) + "($" + to_string(ad2) + ")";
					}
			}
			else if( c == 4 )
			{
				int n = 255;
				int ad2 = n & inst;		
					
				n = n << 8;
				int ad1 = n & inst;
				ad1 = ad1 >> 8;

				n = n << 8;
				int add = n & inst;
				add = add >> 16;
					
				ins = ins + "sw " + "$" + to_string(add) + "," + to_string(ad1) + "($" + to_string(ad2) + ")";
			}
			else if( c == 5 || c == 6 || c == 10 )
			{
				int n = 16383;
				int add = n & inst;

				n = 31;
				n = n << 14;
				int ad2 = n & inst;
				ad2 = ad2 >> 14;
					
				n = n << 5;
				int ad1 = n & inst;
				ad1 = ad1 >> 19;
					
				if( c == 5 )
					ins = ins + "beq";
				else if( c == 6 )
					ins = ins + "bne";
				else
					ins = ins + "blt";

				ins = ins + " $" + to_string(ad1) + ",$" + to_string(ad2) + "," + to_string(add);
			}
			else if( c == 7 )
			{
				int n = 16383;
				int ad = n & inst;

				ins = ins + "j " + to_string(ad) + "\t";
			}
			else if( c == 8 )
			{
				int n = 16383;
				int value1 = n & inst;

				n = 31;
				n = n << 16;

				int ad = n & inst;
				ad = ad >> 16;
				ins = ins + "li $" + to_string(ad) + "," + to_string(value1) + "\t";
			}
			else if( c == 9 )
			{
				int n = 16383;
				int value1 = n & inst;
				n = 31;
				n = n << 16;
				int ad = n & inst;
				ad = ad >> 16;
				ins = ins + "la $" + to_string(ad) + "," + to_string( value1 ) + "\t";
			}
			else if( c == 0 )
			{
					ins = "NULL\t";
			}

			return ins;
		}

};
