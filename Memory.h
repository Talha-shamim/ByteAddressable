#include<iostream>
using namespace std;

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
			for( int i = 0 ; i < 40 ; i+=4 )
			{
				int k = getWord( i );
				int n = 1 << 31;
				
				for( int j = 1 ; j <= 32 ; j++ )
				{
					if( n & k )
						cout << "1 ";
					else
						cout << "0 ";
					if( j % 8 == 0 )
						cout << "	";

					k = k << 1;
				}
				cout << endl;
			}
		}
};
