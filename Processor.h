#include<iostream>
#include"Registers.h"
#include"Memory.h"
#include"ProcessorRegisters.h"
using namespace std;

class Processor
{
	private:
		int clock = 0;
		int PC = 0;
		bool pipeline = false;
	public:
		Registers reg;
		Memory mem;

		void Process( )
		{
			IF_ID_RF iibefore;
			ID_RF_EX iebefore;
			EX_MEM embefore;
			MEM_WB mwbefore;
			WB_IF wibefore;

			IF_ID_RF iiafter;
			ID_RF_EX ieafter;
			EX_MEM emafter;
			MEM_WB mwafter;
			WB_IF wiafter;

			wibefore.completedOrNot = true;
			iibefore.stallOrNot = true;
			iebefore.stallOrNot = true;
			embefore.stallOrNot = true;
			mwbefore.stallOrNot = true;

			while( clock < 10 )
			{
				cout << "clock = " << clock << endl;
//				cout << "PC =  " << PC << endl;

				iiafter = InstructionFetch( wibefore );

//				cout << "iiafter" << endl;
//				cout << "instruction = " << iiafter.instruction << " stallOrNot = " << iiafter.stallOrNot << endl;

				ieafter = InstructionDecodeAndRegisterFetch( iibefore );

//				cout << "ieafter" << endl;
//				cout << "instruction = " << ieafter.instruction << " value1 = " << ieafter.value1 << " value2 = " << ieafter.value2 << " destination = " << ieafter.destination << " stallOrNot = " << ieafter.stallOrNot << endl;

				emafter = Execute( iebefore );

//				cout << "emafter" << endl;
//				cout << "performOrnot = " << emafter.performOrNot << " readOrWrite = " << emafter.readOrWrite << " value = " << emafter.value << " address = " << emafter.address << " stallOrNot = " << emafter.stallOrNot << endl;

				mwafter = MemoryWriteAndRead( embefore );

//				cout << "mwafter" << endl;
//				cout << "performOrNot = " << mwafter.performOrNot << " value = " << mwafter.value << " reg = " << mwafter.reg << " stallOrNot = " << mwafter.stallOrNot << endl;

				wiafter = WriteBack( mwbefore );

//				cout << "writeBack" << endl;
//				cout << "completedOrNot = " << wiafter.completedOrNot << endl;


				iibefore = iiafter;
				wibefore = wiafter;
				embefore = emafter;
				mwbefore = mwafter;
				iebefore = ieafter;

				clock++;
			}
			cout << "PC = " << PC << endl;
		}

		struct IF_ID_RF InstructionFetch( struct WB_IF wi)
		{
			struct IF_ID_RF ii;

			if( wi.completedOrNot == false && pipeline == false )
			{
				ii.stallOrNot = true;
				ii.instruction = 0;
			}
			else
			{
				cout << "IF" << endl;
				ii.stallOrNot = false;
				ii.instruction = mem.getWord( PC );
				PC = PC + 4;
			}
			return ii;
		}

		struct ID_RF_EX InstructionDecodeAndRegisterFetch( struct IF_ID_RF ii )
		{
			struct ID_RF_EX ie;

			if( ii.stallOrNot == true )
			{
				ie.stallOrNot = true;
			}
			else
			{
				cout << "ID/RF" << endl;
				ie.stallOrNot = false;

				int inst = ii.instruction;

				int c = 0;
				int n = 255;
				n = n << 24;
				c = inst & n;
				c = c >> 24;
				ie.instruction = c;

				if( c == 1 )
				{
					int n = 255;
					int ad = n & inst;
					
					if( reg.getRegisterIsUsedOrNot( ad ) == true )
					{
						ie.stallOrNot = true;
						return ie;
					}
					else
						ie.value2 = reg.getRegister( ad );

					n = n << 8;
					ad = n & inst;
					ad = ad >> 8;
					if( reg.getRegisterIsUsedOrNot( ad ) == true )
					{
						ie.stallOrNot = true;
						return ie;
					}
					else
						ie.value1 = reg.getRegister( ad );

					n = n << 8;
					ad = n & inst;
					ad = ad >> 16;
					reg.setRegisterIsUsedOrNot( ad , false );
					ie.destination = ad;
				}
				else if( c == 2 )
				{
					int n = 255;
					int ad = n & inst;
					
					if( reg.getRegisterIsUsedOrNot( ad ) == true )
					{
						ie.stallOrNot = true;
						return ie;
					}
					else
						ie.value2 = reg.getRegister( ad );

					n = n << 8;
					ad = n & inst;
					ad = ad >> 8;
					if( reg.getRegisterIsUsedOrNot( ad ) == true )
					{
						ie.stallOrNot = true;
						return ie;
					}
					else
						ie.value1 = reg.getRegister( ad );

					n = n << 8;
				       	ad = n & inst;
					ad = ad >> 16;
					reg.setRegisterIsUsedOrNot( ad , false );
					ie.destination = ad;
				}
				else if( c == 3 )
				{
					int n = 255;
					int ad = n & inst;
					
					if( reg.getRegisterIsUsedOrNot( ad ) == true )
					{
						ie.stallOrNot = true;
						return ie;
					}
					else
						ie.value2 = reg.getRegister( ad );

					n = n << 8;
					ad = n & inst;
					ad = ad >> 8;
					
					ie.value1 = ad;

					n = n << 8;
					ad = n & inst;
					ad = ad >> 16;
					reg.setRegisterIsUsedOrNot( ad , false );
					ie.destination = ad;
				}
				else if( c == 4 )
				{
					int n = 255;
					int ad = n & inst;
					
					if( reg.getRegisterIsUsedOrNot( ad ) == true )
					{
						ie.stallOrNot = true;
						return ie;
					}
					else
						ie.value2 = reg.getRegister( ad );

					n = n << 8;
					ad = n & inst;
					ad = ad >> 8;
					
					ie.value1 = ad;

					n = n << 8;
					ad = n & inst;
					ad = ad >> 16;
					if( reg.getRegisterIsUsedOrNot( ad ) == true )
					{
						ie.stallOrNot = true;
						return ie;
					}
					else
						ie.destination = reg.getRegister( ad );
				}
				else if( c == 5 )
				{
					int n = 16383;
					int ad = n & inst;
					ie.destination = ad;

					n = 31;
					n = n << 14;
					ad = n & inst;
					ad = ad >> 14;
					if( reg.getRegisterIsUsedOrNot( ad ) == true )
					{
						ie.stallOrNot = true;
						return ie;
					}
					else
						ie.value2 = reg.getRegister( ad );

					n = n << 5;
					ad = n & inst;
					ad = ad >> 19;
					if( reg.getRegisterIsUsedOrNot( ad ) == true )
					{
						ie.stallOrNot = true;
						return ie;
					}
					else
						ie.value1 = reg.getRegister( ad );
				}
				else if( c == 6 )
				{
					int n = 16383;
					int ad = n & inst;
					
					ie.destination = ad;

					n = 31;
					n = n << 14;
					ad = n & inst;
					ad = ad >> 14;
					if( reg.getRegisterIsUsedOrNot( ad ) == true )
					{
						ie.stallOrNot = true;
						return ie;
					}
					else
						ie.value2 = reg.getRegister( ad );

					n = n << 5;
					ad = n & inst;
					ad = ad >> 19;
					if( reg.getRegisterIsUsedOrNot( ad ) == true )
					{
						ie.stallOrNot = true;
						return ie;
					}
					else
						ie.value1 = reg.getRegister( ad );
				}
				else if( c == 7 )
				{
					int n = 16383;
					int ad = n & inst;

					ie.destination = ad;
					ie.value1 = 0;
					ie.value2 = 0;
				}
			}
			return ie;
		}

		struct EX_MEM Execute( struct ID_RF_EX ie )
		{
			struct EX_MEM em;

			if( ie.stallOrNot == true )
			{
				em.stallOrNot = true;
			}
			else
			{
				cout << "EX" << endl;
				em.stallOrNot = false;

				int inst = ie.instruction;
				if( inst == 1 )
				{
					em.value = ie.value1 + ie.value2;
					em.address = ie.destination;
					em.performOrNot = false;
				}
				else if( inst == 2 )
				{
					em.value = ie.value1 - ie.value2;
					em.address = ie.destination;
					em.performOrNot = false;
				}
				else if( inst == 3 )
				{
					em.address = ie.value1 + ie.value2;
					em.value = ie.destination;
					em.performOrNot = true;
					em.readOrWrite = false;
				}
				else if( inst == 4 )
				{
					em.address = ie.value1 + ie.value2;
					em.value = ie.destination;
					em.performOrNot = true;
					em.readOrWrite = true;
				}
				else if( inst == 5 )
				{
					if( ie.value1 == ie.value2 )
						em.value = ie.destination;
					else
						em.value = -1;
					em.address = 32;
					em.performOrNot = false;
				}
				else if( inst == 6 )
				{
					if( ie.value1 != ie.value2 )
						em.value = ie.destination;
					else
						em.value = -1;
					em.address = 32;
					em.performOrNot = false;
				}
				else if( inst == 7 )
				{
					em.performOrNot = false;
					em.address = 32;
					em.value = ie.destination;
				}
			}
			return em;
		}

		struct MEM_WB MemoryWriteAndRead( struct EX_MEM em )
		{
			struct MEM_WB mw;
			if( em.stallOrNot == true )
			{
				mw.stallOrNot = true;
			}
			else
			{
				cout << "MEM" << endl;
				mw.stallOrNot = false;
				if( em.performOrNot == true )
				{
					if( em.readOrWrite == false )
					{
						mw.value = mem.getWord( em.address );
						mw.reg = em.value;
						mw.performOrNot = true;
					}
					else
					{
						mem.writeWord( em.value , em.address );
						mw.performOrNot =  false;
					}
				}
				else
				{
					mw.value = em.value;
					mw.reg = em.address;
					mw.performOrNot = true;
				}
			}
			return mw;
		}

		struct WB_IF WriteBack( struct MEM_WB mw )
		{
			struct WB_IF wi;
			if( mw.stallOrNot == true )
			{
				wi.completedOrNot = false;
			}
			else
			{
				cout << "WB" << endl;
				if( mw.performOrNot == false )
				{
					wi.completedOrNot = true;
				}
				else
				{
					wi.completedOrNot = true;
					if( mw.reg == 32 )
					{
						if( mw.value == -1 )
						{
							wi.completedOrNot = true;
						}
						else
						{
							PC = mw.value;
						}
					}
					else
					{
						reg.setRegister( mw.reg , mw.value );
						reg.setRegisterIsUsedOrNot( mw.reg , false );
					}
				}
			}
			return wi;
		}
};
