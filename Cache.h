#include<iostream>
#include<queue>
#include<cmath>
#include<stdlib.h>
#include"ProcessorRegisters.h"
using namespace std;
int coldMisses = 0;
int L1hits = 0;
int L1misses = 0;
int L2hits = 0;
int L2misses = 0;
int bufferhits = 0;
int buffermisses = 0;
int storel1hits = 0;
int storel2hits = 0;
int storebufferhits = 0;
int storememoryhits = 0;
int writtenback = 0;

class StallNumber
{
	public:
		int value = 0;
		int stalls = 0;
};

class Block
{
	public:
		int tag = -1;
		unsigned char* b;
		int fifo = -1;
		bool dirty = false;
		int lru = -1;
		int lfu = -1;
		int stalls = 0;

		Block( int size )
		{
			unsigned char *cr = (unsigned char*)calloc( size , sizeof( unsigned char ));
			b = cr;
		}
};
queue<Block> buffer;

class Set
{
	public:
		Block* b;
};

class Cache
{
	public:
		int L1size = 0;
		int L2size = 0;
		int BlockSize = 0;
		int NoOfSetsL1 = 0;
		int NoOfSetsL2 = 0;
		int NoOfIndexL1bits = 0;
		int NoOfIndexL2bits = 0;
		int NoOfOffsetL1bits = 0;
		int NoOfOffsetL2bits = 0;
		int Associativity = 0;
		int l1time = 0;
		int l2time = 0;
		int buffertime = 0;
		int memorytime = 0;
		int policy = 0;
		Set* L1Cache;
		Set* L2Cache;
	
		int getBlockSize()
		{
			return BlockSize;
		}

		Cache( int l1 , int l2 , int b , int a , Set* l1cache , Set* l2cache , int rp , int l1t , int l2t , int mt , int bt )
		{
			L1size = l1;
			L2size = l2;
			BlockSize = b;
			Associativity = a;
			L1Cache = l1cache;
			L2Cache = l2cache;
			l1time = l1t;
			l2time = l2t;
			memorytime = mt;
			buffertime = bt;
			policy = rp;
			int n = l1/b;
			NoOfSetsL1 = n/a;

			n = l2/b;
			NoOfSetsL2 = n/a;

			for( int i = 0 ; i < NoOfSetsL1 ; i++ )
			{
				Block* b1 = (Block*)calloc( a , sizeof( Block ) );
				L1Cache[i].b = b1;
			}

			for( int i = 0 ; i < NoOfSetsL2 ; i++ )
			{
				Block* b1 = (Block*)calloc( a , sizeof( Block ) );
				L2Cache[i].b = b1;
			}

			for( int i = 0 ; i < NoOfSetsL1 ; i++ )
			{
				for( int j = 0 ; j < a ; j++ )
				{
					unsigned char* arr  = ( unsigned char* ) calloc( BlockSize , sizeof( unsigned char ) );
					L1Cache[i].b[j].b = arr;
					L1Cache[i].b[j].tag = -1;
					L1Cache[i].b[j].lru = -1;
					L1Cache[i].b[j].lfu = -1;
					L1Cache[i].b[j].fifo = -1;
				}
			}

			for( int i = 0 ; i < NoOfSetsL2 ; i++ )
			{
				for( int j = 0 ; j < a ; j++ )
				{
					unsigned char* arr  = ( unsigned char* ) calloc( BlockSize , sizeof( unsigned char ) );
					L2Cache[i].b[j].b = arr;
					L2Cache[i].b[j].tag = -1;
					L2Cache[i].b[j].lru = -1;
					L2Cache[i].b[j].lfu = -1;
					L2Cache[i].b[j].fifo = -1;
				}
			}

			NoOfIndexL1bits = (int)log2(NoOfSetsL1);
			NoOfIndexL2bits = (int)log2(NoOfSetsL2);
			NoOfOffsetL1bits = (int)log2(BlockSize);
			NoOfOffsetL2bits = (int)log2(BlockSize);
		}

		int getSetNumberL1( int address )
		{
			int a = NoOfSetsL1-1;
			a = a << NoOfOffsetL1bits;
			int b = a & address;
			b = b >> NoOfOffsetL1bits;
			return b;
		}

		int getSetNumberL2( int address )
		{
			int a = NoOfSetsL2-1;
			a = a << NoOfOffsetL2bits;
			int b = a & address;
			b = b >> NoOfOffsetL2bits;
			return b;
		}

		int getBlockNumberL1( int address )
		{
			int a = BlockSize-1;
			int b = a & address;
			return b;
		}

		int getBlockNumberL2( int address )
		{
			int a = BlockSize-1;
			int b = a & address;
			return b;
		}

		int getTagL1( int address )
		{
			int a = NoOfIndexL1bits + NoOfOffsetL1bits;
			address = address >> a;
			return address;
		}

		int getTagL2( int address )
		{
			int a = NoOfIndexL2bits + NoOfOffsetL2bits;
			address = address >> a;
			return address;
		}

		StallNumber Load( int address , int NoOfbytes )
		{
			int setnum = getSetNumberL1( address );
			int tagnum = getTagL1( address );

			for( int i = 0 ; i < Associativity ; i++ )
			{
				if( L1Cache[setnum].b[i].tag == tagnum )
				{
					int value = 0;
					for( int j = 1 ; j <= NoOfbytes ; j++ )
					{
						value = value << 8;
						value = value | L1Cache[setnum].b[i].b[getBlockNumberL1( address ) + j - 1 ];
					}
				
					L1Cache[setnum].b[i].lfu = L1Cache[setnum].b[i].lfu + 1;
					
					int max = 0;
					for( int j = 0 ; j < Associativity ; j++ )
					{
						if( L1Cache[setnum].b[j].lru > max )
							max = L1Cache[setnum].b[j].lru;
					}

					for( int j = 0 ; j < Associativity ; j++ )
					{
						if( L1Cache[setnum].b[i].lru < L1Cache[setnum].b[j].lru )
							L1Cache[setnum].b[j].lru = L1Cache[setnum].b[j].lru - 1;
					}
					
					L1Cache[setnum].b[i].lru = max;
					StallNumber sn;
					sn.stalls = 0;
					sn.value = value;
					L1hits++;
					return sn;
				}
			}
			L1misses++;

			int value = 0;
			StallNumber sn;

			Block br = LoadL2( address );
			Block b(BlockSize);
			b.tag = br.tag;
			b.lru = br.lru;
			b.lfu = br.lfu;
			b.fifo = br.fifo;
			for( int j = 0 ; j < BlockSize ; j++ )
				b.b[j] = br.b[j];
			if( b.tag == -1 )
			{
				L2misses++;
				Block ret(BlockSize);
				ret.tag = -2;

				queue<Block> buff = buffer;
				while( !buffer.empty() )
					buffer.pop();
				bool b = false;
				while( !buff.empty() )
				{
					if( buff.front().tag == address )
					{
						for( int j = 0 ; j < BlockSize ; j++ )
							ret.b[j] = buff.front().b[j];
						ret.tag = buff.front().tag;
						buff.pop();
					}
					else
					{
						buffer.push(buff.front() );
						buff.pop();
					}
				}

				Block block( BlockSize );
				if( ret.tag != -2 )
				{
					bufferhits++;
					sn.stalls = l2time + buffertime;
					for( int i = 0 ; i < BlockSize ; i++ )
						block.b[i] = ret.b[i];
				}
				else
				{
					buffermisses++;
					sn.stalls = l2time + buffertime + memorytime;
					int a = address >> NoOfOffsetL1bits;
					a = a << NoOfOffsetL1bits;
					for( int i = 0 ; i < BlockSize ; i++ )
					{
						block.b[i] = mem.getByte( a + i );
					}
				}
				value = 0;
				int k = getBlockNumberL1( address );
				for( int j = 1 ; j <= NoOfbytes ; j++ )
				{
					value = value << 8;
					value = value | block.b[ k + j - 1 ];
				}

				Block rL1 = InsertBlockInL1( block , address );
				Block returnedblockbyL1( BlockSize );
				returnedblockbyL1.tag = rL1.tag;
				returnedblockbyL1.dirty = rL1.dirty;
				for( int j = 0 ; j < BlockSize ; j++ )
					returnedblockbyL1.b[j] = rL1.b[j];

				if( returnedblockbyL1.tag != -1 )
				{
					Block rL2 = InsertBlockInL2( returnedblockbyL1 , returnedblockbyL1.tag );
					Block returnedblockbyL2( BlockSize );
					for( int j = 0 ; j < BlockSize ; j++ )
						returnedblockbyL2.b[j] = rL2.b[j];
					returnedblockbyL2.tag = rL2.tag;
					returnedblockbyL2.dirty = rL2.dirty;

					if( returnedblockbyL2.tag != -1  and returnedblockbyL2.dirty )
					{
						queue<Block> buff = buffer;
						while( !buffer.empty() )
							buffer.pop();
						bool b = false;
						while( !buff.empty() )
						{
							if( buff.front().tag == returnedblockbyL2.tag )
							{
								b = true;
								for( int j = 0 ; j < BlockSize ; j++ )
									buff.front().b[j] = returnedblockbyL2.b[j];
								buff.front().stalls = 100;
							}
							buffer.push(buff.front() );
							buff.pop();
						}
						if( b == false )
						{
							Block temp(BlockSize);
							temp.tag = returnedblockbyL2.tag;
							for( int i = 0 ; i < BlockSize ; i++ )
								temp.b[i] = returnedblockbyL2.b[i];
						        temp.stalls = 100;
							buffer.push(temp);
						}
						writtenback++;
					}
				}	
				else
					coldMisses++;
			}
			else
			{
				L2hits++;
				sn.stalls = l2time;
				value = 0;
				for( int j = 1 ; j <= NoOfbytes ; j++ )
				{
					value = value << 8;
					value = value | b.b[getBlockNumberL2( address ) + j - 1 ];
				}
				Block rL1 = InsertBlockInL1( b , b.tag );
				Block returnedblockbyL1( BlockSize );
				returnedblockbyL1.tag = rL1.tag;
				returnedblockbyL1.dirty = rL1.dirty;
				for( int j = 0 ; j < BlockSize ; j++ )
					returnedblockbyL1.b[j] = rL1.b[j];

				if( returnedblockbyL1.tag != -1 )
				{
					Block rL2 = InsertBlockInL2( returnedblockbyL1 , returnedblockbyL1.tag );
					Block returnedblockbyL2( BlockSize );
					for( int j = 0 ; j < BlockSize ; j++ )
						returnedblockbyL2.b[j] = rL2.b[j];
					returnedblockbyL2.tag = rL2.tag;
					returnedblockbyL2.dirty = rL2.dirty;

					if( returnedblockbyL2.tag != -1 and returnedblockbyL2.dirty )
					{
						queue<Block> buff = buffer;
						while( !buffer.empty() )
						buffer.pop();
						bool b = false;
						while( !buff.empty() )
						{
							if( buff.front().tag == returnedblockbyL2.tag )
							{
								b = true;
								for( int j = 0 ; j < BlockSize ; j++ )
									buff.front().b[j] = returnedblockbyL2.b[j];
								buff.front().stalls = 100;
							}
							buffer.push(buff.front() );
							buff.pop();
						}
						if( b == false )
						{
							Block temp(BlockSize);
							temp.tag = returnedblockbyL2.tag;
							for( int i = 0 ; i < BlockSize ; i++ )
								temp.b[i] = returnedblockbyL2.b[i];
							temp.stalls = 100;
							buffer.push(temp);
						}
						writtenback++;					
					}
				}
			}
			sn.value = value;
			return sn;
		}

		Block LoadL2( int address )
		{
			int setnum = getSetNumberL2( address );
			int tagnum = getTagL2( address );

			for( int i = 0 ; i < Associativity ; i++ )
			{
				if( L2Cache[setnum].b[i].tag == tagnum )
				{
					Block value( BlockSize );
					for( int j = 0 ; j < BlockSize ; j++ )
						value.b[j] = L2Cache[setnum].b[i].b[j];

					value.tag = tagnum;
					value.tag = value.tag << NoOfIndexL2bits;
					value.tag = value.tag + setnum;
					value.tag = value.tag << NoOfOffsetL2bits;

					L2Cache[setnum].b[i].tag = -1;
					L2Cache[setnum].b[i].lfu = -1;
					for( int j = 0 ; j < BlockSize ; j++ )
						L2Cache[setnum].b[i].b[j] = 0;

					for( int j = 0 ; j < Associativity ; j++ )
					{
						if( L2Cache[setnum].b[j].fifo > L2Cache[setnum].b[i].fifo )
							L2Cache[setnum].b[j].fifo = L2Cache[setnum].b[j].fifo-1;
						if( L2Cache[setnum].b[j].lru > L2Cache[setnum].b[i].lru )
							L2Cache[setnum].b[j].lru = L2Cache[setnum].b[j].lru-1;
					}

					L2Cache[setnum].b[i].fifo = -1;
					L2Cache[setnum].b[i].lru = -1;
					return value;
				}
			}

			Block b( BlockSize );
			b.tag = -1;
			return b;
		}

		Block InsertBlockInL1( Block block , int address )
		{
			int setnum = getSetNumberL1( address );
			Block returnblock( BlockSize );
			int blockaddress = -1;

			for( int i = 0 ; i < Associativity ; i++ )
			{
				if( L1Cache[setnum].b[i].tag == -1 )
				{
					blockaddress = i;
					break;
				}
			}
			
			if( blockaddress == -1 )
			{
				blockaddress = ReplaceL1( address );
				returnblock.tag = L1Cache[setnum].b[blockaddress].tag;
				returnblock.tag = returnblock.tag << NoOfIndexL1bits;
				returnblock.tag = returnblock.tag + setnum;
				returnblock.tag = returnblock.tag << NoOfOffsetL1bits;

				for( int j = 0 ; j < BlockSize ; j++ )
					returnblock.b[j] = L1Cache[setnum].b[blockaddress].b[j];
				returnblock.dirty = L1Cache[setnum].b[blockaddress].dirty;
			}
			else
			{
				returnblock.tag = L1Cache[setnum].b[blockaddress].tag;

				for( int j = 0 ; j < BlockSize ; j++ )
					returnblock.b[j] = L1Cache[setnum].b[blockaddress].b[j];
				returnblock.dirty = L1Cache[setnum].b[blockaddress].dirty;
			}
			

			L1Cache[setnum].b[blockaddress].tag = getTagL1( address );
			L1Cache[setnum].b[blockaddress].lfu = 1;
			L1Cache[setnum].b[blockaddress].dirty = block.dirty;

			for( int j = 0 ; j < BlockSize ; j++ )
			{
				L1Cache[setnum].b[blockaddress].b[j] = block.b[j];
			}

			if( L1Cache[setnum].b[blockaddress].lru != -1 and L1Cache[setnum].b[blockaddress].fifo != -1 )
			{
				for( int j = 0 ; j < Associativity ; j++ )
				{
					if( L1Cache[setnum].b[j].fifo > L1Cache[setnum].b[blockaddress].fifo )
						L1Cache[setnum].b[j].fifo = L1Cache[setnum].b[j].fifo - 1;
					
					if( L1Cache[setnum].b[j].lru > L1Cache[setnum].b[blockaddress].lru )
						L1Cache[setnum].b[j].lru = L1Cache[setnum].b[j].lru - 1;
				}
			}
			L1Cache[setnum].b[blockaddress].lru = -1;
			L1Cache[setnum].b[blockaddress].fifo = -1;

			int max = 0;
			for( int j = 0 ; j < Associativity ; j++ )
			{
				if( L1Cache[setnum].b[j].fifo > L1Cache[setnum].b[max].fifo )
					max = j;
			}

			if( L1Cache[setnum].b[max].fifo == -1 )
					L1Cache[setnum].b[blockaddress].fifo = 1;
			else
				L1Cache[setnum].b[blockaddress].fifo = L1Cache[setnum].b[max].fifo + 1;

			max = 0;
			for( int j = 0 ; j < Associativity ; j++ )
			{
				if( L1Cache[setnum].b[j].lru > L1Cache[setnum].b[max].lru )
					max = j;
			}
			if( L1Cache[setnum].b[max].lru == -1 )
				L1Cache[setnum].b[blockaddress].lru = 1;
			else
				L1Cache[setnum].b[blockaddress].lru = L1Cache[setnum].b[max].lru+1;

			return returnblock;
		}

		Block InsertBlockInL2( Block block , int address )
		{
			int setnum = getSetNumberL2( address );
			Block returnblock(BlockSize);
			int blockaddress = -1;

			for( int i = 0 ; i < Associativity ; i++ )
			{
				if( L2Cache[setnum].b[i].tag == -1 )
				{
					blockaddress = i;
					break;
				}
			}
			if( blockaddress == -1 )
			{
				blockaddress = ReplaceL2( address );
				returnblock.tag = L2Cache[setnum].b[blockaddress].tag;
				for( int j = 0 ; j < BlockSize ; j++ )
					returnblock.b[j] = L2Cache[setnum].b[blockaddress].b[j];
				returnblock.tag = returnblock.tag << NoOfIndexL2bits;
				returnblock.tag = returnblock.tag + setnum;
				returnblock.tag = returnblock.tag << NoOfOffsetL2bits;

				returnblock.dirty = L2Cache[setnum].b[blockaddress].dirty;
			}
			else
			{
				returnblock.tag = L2Cache[setnum].b[blockaddress].tag;

				for( int j = 0 ; j < BlockSize ; j++ )
					returnblock.b[j] = L2Cache[setnum].b[blockaddress].b[j];
				returnblock.dirty = L2Cache[setnum].b[blockaddress].dirty;
			}

			L2Cache[setnum].b[blockaddress].tag = getTagL2( address );
			L2Cache[setnum].b[blockaddress].lfu = 1;
			L2Cache[setnum].b[blockaddress].dirty = block.dirty;

			for( int j = 0 ; j < BlockSize ; j++ )
			{
				L2Cache[setnum].b[blockaddress].b[j] = block.b[j];
			}

			if( L2Cache[setnum].b[blockaddress].fifo != -1 and L2Cache[setnum].b[blockaddress].lru != -1 )
			{
				for( int j = 0 ; j < Associativity ; j++ )
				{
					if( L2Cache[setnum].b[j].fifo > L2Cache[setnum].b[blockaddress].fifo )
						L2Cache[setnum].b[j].fifo = L2Cache[setnum].b[j].fifo - 1;
					
					if( L2Cache[setnum].b[j].lru > L2Cache[setnum].b[blockaddress].lru )
						L2Cache[setnum].b[j].lru = L2Cache[setnum].b[j].lru - 1;
				}
			}
			L2Cache[setnum].b[blockaddress].lru = -1;
			L2Cache[setnum].b[blockaddress].fifo = -1;

			int max = 0;
			for( int j = 0 ; j < Associativity ; j++ )
			{
				if( L2Cache[setnum].b[j].fifo > L2Cache[setnum].b[max].fifo )
					max = j;
			}

			if( L2Cache[setnum].b[max].fifo == -1 )
					L2Cache[setnum].b[blockaddress].fifo = 1;
			else
				L2Cache[setnum].b[blockaddress].fifo = L2Cache[setnum].b[max].fifo + 1;

			max = 0;
			for( int j = 0 ; j < Associativity ; j++ )
			{
				if( L2Cache[setnum].b[j].lru > L2Cache[setnum].b[max].lru )
					max = j;
			}
			if( L2Cache[setnum].b[max].lru == -1 )
				L2Cache[setnum].b[blockaddress].lru = 1;
			else
				L2Cache[setnum].b[blockaddress].lru = L2Cache[setnum].b[max].lru+1;

			return returnblock;
		}

		StallNumber Store( int address , int number , int NoOfBytes )
		{
			int set1 = getSetNumberL1( address );
			int tag1 = getTagL1( address );

			for( int i = 0 ; i < Associativity ; i++ )
			{
				if( L1Cache[set1].b[i].tag == tag1 )
				{
					L1Cache[set1].b[i].dirty = true;
					int n = number;
	
					for( int j = NoOfBytes ; j >= 1 ; j-- )
					{
						L1Cache[set1].b[i].b[ getBlockNumberL1( address ) + j - 1 ] = 0 | n;
						n =  n >> 8;
					}

					StallNumber sn;
					sn.stalls = 0;
					storel1hits++;
					return sn;
				}
			}

			set1 = getSetNumberL2(address);
			tag1 = getTagL2( address );

			for( int i = 0 ; i < Associativity ; i++ )
			{
				if( L2Cache[set1].b[i].tag == tag1 )
				{
					L2Cache[set1].b[i].dirty = true;
					int n = number;

					for( int j = NoOfBytes ; j >= 1 ; j-- )
					{
						L2Cache[set1].b[i].b[ getBlockNumberL2( address ) + j - 1 ] = 0 | n;
						n = n >> 8;
					}
				        StallNumber sn;	
					sn.stalls = l2time;
					storel2hits++;
					return sn;
				}
			}
			
			int n = number;
			
			queue<Block> buff = buffer;
			while( !buffer.empty() )
				buffer.pop();
			bool b = false;
			while( !buff.empty() )
			{
				if( buff.front().tag == address )
				{
					b = true;
					for( int j = NoOfBytes ; j >= 1 ; j-- )
					{
						buff.front().b[ getBlockNumberL2(address) + j -1 ] = 0 | n;
						n = n >> 8;
					}
					buff.front().stalls = 100;
				}
				buffer.push(buff.front());
				buff.pop();
			}
			if( b == true )
			{
				StallNumber sn;
				storebufferhits++;
				sn.stalls = l2time + buffertime;
				return sn;
			}

			storememoryhits++;
			mem.writeWord( number , address );
			StallNumber sn;
			sn.stalls = l2time + buffertime + memorytime;
			return sn;
		}


		int ReplaceL1( int address )
		{
			if( policy == 1 )
			{
				int setnum = getSetNumberL1( address );
				for( int i = 0 ; i < Associativity ; i++ )
				{
					if( L1Cache[setnum].b[i].lru == 1 )
						return i;
				}
			}
			else if( policy == 2 )
			{
				int setnum = getSetNumberL1( address );
				int min = 100;
				int minindex = 0;
				for( int i = 0 ; i < Associativity ; i++ )
				{
					if( L1Cache[setnum].b[i].lfu < min )
					{
						min = L1Cache[setnum].b[i].lfu;
						minindex = i;
					}
				}
				return minindex;
			}
			else if( policy == 3 )
			{
				int setnum = getSetNumberL1( address );
				for( int i = 0 ; i < Associativity ; i++ )
				{
					if( L1Cache[setnum].b[i].fifo == 1 )
						return i;
				}
			}
			else if( policy == 4 )
			{
				return rand()%Associativity;
			}
			else if( policy == 5 )
			{
				int setnum = getSetNumberL1( address );
				int min = 1000;
				int minindex = 0;
				int n;
				for( int i = 0 ; i < Associativity ; i++ )
				{
					n = L1Cache[setnum].b[i].lru + L1Cache[setnum].b[i].lru + L1Cache[setnum].b[i].fifo;
					if( n < min )
					{
						min = n;
						minindex = i;
					}
				}
				return minindex;
			}
			return 0;
		}

		int ReplaceL2( int address )
		{
			if( policy == 1 )
			{
				int setnum = getSetNumberL2( address );
				for( int i = 0 ; i < Associativity ; i++ )
				{
					if( L2Cache[setnum].b[i].lru == 1 )
						return i;
				}
			}
			else if( policy == 2 )
			{
				int setnum = getSetNumberL2( address );
				int min = 100;
				int minindex = 0;
				for( int i = 0 ; i < Associativity ; i++ )
				{
					if( L2Cache[setnum].b[i].lfu < min )
					{
						min = L2Cache[setnum].b[i].lfu;
						minindex = i;
					}
				}
				return minindex;
			}
			else if( policy == 3 )
			{
				int setnum = getSetNumberL2( address );
				for( int i = 0 ; i < Associativity ; i++ )
				{
					if( L2Cache[setnum].b[i].fifo == 1 )
						return i;
				}
			}
			else if( policy == 4 )
			{
				return rand()%Associativity;
			}
			else if( policy == 5 )
			{
				int setnum = getSetNumberL2( address );
				int min = 1000;
				int minindex = 0;
				int n;
				for( int i = 0 ; i < Associativity ; i++ )
				{
					n = L2Cache[setnum].b[i].lru + L2Cache[setnum].b[i].lru + L2Cache[setnum].b[i].fifo;
					if( n < min )
					{
						min = n;
						minindex = i;
					}
				}
				return minindex;
			}
			return 0;
		}

		void flushCache()
		{
			int address;
			for( int i = 0 ; i < NoOfSetsL1 ; i++ )
			{
				for( int j = 0 ; j < Associativity ; j++ )
				{
					if( L1Cache[i].b[j].tag != -1 )
					{
						address = L1Cache[i].b[j].tag;
						address = address << NoOfIndexL1bits;
						address = address | i;
						address = address << NoOfOffsetL1bits;
						L1Cache[i].b[j].tag = address;
						buffer.push( L1Cache[i].b[j] );
						L1Cache[i].b[j].tag = L1Cache[i].b[j].tag >> ( NoOfOffsetL1bits + NoOfIndexL1bits );
					}
				}
			}

			for( int i = 0 ; i < NoOfSetsL2 ; i++ )
			{
				for( int j = 0 ; j < Associativity ; j++ )
				{
					if( L2Cache[i].b[j].tag != -1 )
					{
						address = L2Cache[i].b[j].tag;
						address = address << NoOfIndexL2bits;
						address = address | i;
						address = address << NoOfOffsetL2bits;
						L2Cache[i].b[j].tag = address;
						buffer.push( L2Cache[i].b[j] );
						L2Cache[i].b[j].tag = L2Cache[i].b[j].tag >> ( NoOfOffsetL2bits + NoOfIndexL2bits );
					}
				}
			}
		}

		void writebackbuffer()
		{
			int address = 0;
			int value = 0;
			queue<Block> buff = buffer;
			while( !buff.empty() )
			{
				address = buff.front().tag;
				for( int i = 0 ;  i < BlockSize ; i++ )
				{
					mem.writeByte( buff.front().b[i] , address + i );
				}
				buff.pop();
			}
		}
};
