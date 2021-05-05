#include<iostream>
#include"Cache.h"
using namespace std;
class Branch
{
	public:
		int address = -1;
		bool branchOrNot = false;
};

class BranchPredictor
{
	private:
		Branch array[16];
		
	public:
		int getPC( int ad )
		{
			int n = 31;
			n = n << 24;
			n = n & mem.getWord(ad);
			n = n >> 24;
			if( n != 7 and n != 17 and n != 5 and n != 6 and n != 10 and n != 15 )
				return ad+4;

			for( int i = 0 ; i < 16 ; i++ )
			{
				if( array[i].address == ad )
				{
					if( array[i].branchOrNot == false )
					{
						return ad+4;
					}
					else
					{
						int n = 16383;
						n = n & mem.getWord( ad );
						return n;
					}
				}
			}

			for( int i = 0 ; i < 16 ; i++ )
			{
				if( array[i].address == -1 )
				{
					array[i].address = ad;
					int target = 16383;
					target = target & mem.getWord( ad );
					
					int n = 31;
					n = n << 24;
					int instruction = n & mem.getWord( ad );
					instruction = instruction >> 24;
					
					if( instruction == 7 or instruction == 17 )
					{
						array[i].branchOrNot = true;
						return target;
					}	
					else
					{
						if( target < ad )
						{
							array[i].branchOrNot = true;
							return target;
						}
						else if( target > ad )
						{
							array[i].branchOrNot = false;
							return ad + 4;
						}
					}
				}
			}
			return 0;
		}
};
