#include<iostream>
#include<fstream>
#include"ProcessorRegisters.h"
using namespace std;
string s[3000][3000];

class Processor
{
	private:
		int clock = 1;
		int PC = 8192;
		bool pipeline = true;
		bool OperandForwarding = false;
		bool EarlyBranchResolution = false;
		bool stepwiseExecution = true;
	public:
		string Process( bool pipe , bool oper , bool early , bool step )
		{
			pipeline = pipe;
			OperandForwarding = oper;
			EarlyBranchResolution = early;
			stepwiseExecution = step;
			if( stepwiseExecution )
				pipeline = false;

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

			wibefore.OperateOrNot = true;
			wibefore.completedOrNot = true;
			iibefore.OperateOrNot = false;
			iebefore.OperateOrNot = false;
			embefore.OperateOrNot = false;
			mwbefore.OperateOrNot = false;

			for( int i = 1 ; i < 3000 ; i++ )
			{
				for( int j = 0 ; j < 3000 ; j++ )
					s[i][j] = "       ";
			}

			for( int i = 0 ; i < 3000 ; i++ )
					s[i][0] = "NOP    \t";

			int n = 16383;
			int ad = n & mem.getWord(PC);
			PC = ad;
	
			int g = 1;
			int inscnt = 1;
			bool stop = false;
			int NoOfConditionalBranches = 0;
			int NoOfUnconditionalBranches = 0;
			int NoOfLoadStores = 0;
			int NoOfArithmetic = 0;
			int NoOfBranchesTaken = 0;
			int NoOfStallsBecauseOfBranches = 0;
			int NoOfStallsBecauseOfDataDependency = 0;
			int TotalNoOfInstructionsExecuted = 0;
			while( true )
			{
				
				if( stepwiseExecution )
				{
					if( g == 1 )
					{
						string s;
						cout << "Enter 'e' and Press 'Enter' to Execute the First Instruction" << endl;
						cout << "\e[33m";
						cin >> s;
						cout << "\e[0m";
						if( s != "e" )
						{
							cout << "Enter correct values and try again" << endl;
							continue;
						}
						else
						{
							g++;
						}
					}
					else if( ( g % 5 ) == 1 )
					{
						if( wibefore.completedOrNot and !stop )
						{
							cout << "PC = " << wiafter.pc << endl;
							Print( wiafter.reg , wiafter.pc , wiafter.mem );
						}
						string s;
						cout << "Enter 'e' and Press 'Enter' to Execute the Next Instruction (OR)" << endl;
						cout << "Enter 'c' and Press 'Enter' to Execute the Remaining Instructions Till the End (OR)" << endl;
						cout << "Enter 'b' and Press 'Enter' to Terminate the Execution and go back to the main menu" << endl;
						cout << "\e[33m";
						cin >> s;
						cout << "\e[0m";
						if( s == "e" )
						{
							g++;
							stop = false;
						}
						else if( s == "b" )
						{
							stop = false;
							cout << "Program Terminated abruptly" << endl;
							return NULL;
						}
						else if( s == "c" )
						{
							stop = false;
							stepwiseExecution = false;
						}
						else
						{
							cout << "!!Enter correct values and try again" << endl;
							stop = true;
							continue;
						}
					}
					else
						g++;
				}

				IF_ID_RF temp = InstructionFetch( wibefore , inscnt );
				
				if ( pipeline == false )
				{
					iiafter = temp;
					if( iiafter.OperateOrNot == true )
						inscnt++;
				}
				else
				{
					if( temp.OperateOrNot == true )
					{
						iiafter = temp;
						inscnt++;
					}
				}

				ID_RF_EX tempp = InstructionDecodeAndRegisterFetch( iibefore , OperandForwarding );
				
				if( tempp.OperateOrNot == false and emafter.stallOrNot == true )
				{
				}
				else
					ieafter = tempp;
				if( tempp.OperateOrNot )
				{
					s[ieafter.inscnt][0] = ieafter.ins;
				}

				
				
				emafter = Execute( iebefore , embefore , mwbefore , wibefore );

				if( emafter.branchOrNot and !pipeline)
					NoOfBranchesTaken++;
				if( emafter.branchOrNot and pipeline == true  and emafter.stallOrNot == false )
				{
					if( ieafter.stallOrNot == true )
						NoOfStallsBecauseOfDataDependency--;
					ieafter.OperateOrNot = false;
					ieafter.stallOrNot = false;
					if( ieafter.regSetToUsed != -1 )
						reg.setRegisterIsUsedOrNot( ieafter.regSetToUsed , false );
					ID_RF_EX temp = InstructionDecodeAndRegisterFetch( iiafter , OperandForwarding );

					s[temp.inscnt][0] = temp.ins;
					string sq = s[temp.inscnt-1][0];
					s[temp.inscnt-1][0] = "sq-" + sq;
					sq = s[temp.inscnt][0];
					s[temp.inscnt][0] = "sq-" + sq;
					if( temp.regSetToUsed != -1 )
						reg.setRegisterIsUsedOrNot( temp.regSetToUsed , false );
					iiafter.OperateOrNot = false;
					iiafter.stallOrNot = false;
					s[ieafter.inscnt][clock] = " ID/RF ";
					NoOfStallsBecauseOfBranches = NoOfStallsBecauseOfBranches + 2;
					NoOfBranchesTaken++;
				}

				mwafter = MemoryWriteAndRead( embefore );

				wiafter = WriteBack( mwbefore );

				if( temp.OperateOrNot )
				{
					s[temp.inscnt][clock] = "  IF   ";
				}
				
				if( tempp.stallOrNot )
				{
					s[tempp.inscnt][clock] = " stall ";
					NoOfStallsBecauseOfDataDependency++;
				}
				
				if( tempp.OperateOrNot  and !tempp.stallOrNot )
				{
					s[tempp.inscnt][clock] = " ID/RF ";
				}
				
				if( emafter.stallOrNot )
				{
					s[emafter.inscnt][clock] = " stall ";
					NoOfStallsBecauseOfDataDependency++;
				}
				if( emafter.OperateOrNot and !emafter.stallOrNot )
				{
					s[emafter.inscnt][clock] = "  EX   ";
				}
				
				if( mwafter.OperateOrNot )
				{
					s[mwafter.inscnt][clock] = "  MEM  ";
				}
				
				if( wiafter.completedOrNot )
				{
					s[wiafter.inscnt][clock] = "  WB   ";
					if( wiafter.instruction == 1 || wiafter.instruction == 2 || wiafter.instruction == 8 || wiafter.instruction == 9 )
						NoOfArithmetic++;
					else if( wiafter.instruction == 3 || wiafter.instruction == 4 )
						NoOfLoadStores++;
					else if( wiafter.instruction == 5 || wiafter.instruction == 6 || wiafter.instruction == 10 )
						NoOfConditionalBranches++;
					else if( wiafter.instruction == 7 )
						NoOfUnconditionalBranches++;
				}
				IF_ID_RF tempr = iibefore;

				if( ieafter.stallOrNot )
				{
					wiafter.OperateOrNot = false;
				}
				else
				{
					iibefore = iiafter;
				}

				if( emafter.stallOrNot )
				{
					wiafter.OperateOrNot = false;
					iibefore = tempr;
				}
				else
				{
					iebefore = ieafter;
				}

				embefore = emafter;
				wibefore = wiafter;
				mwbefore = mwafter;
			
				if( wiafter.stop )
				{
					cout << "Please Wait While the Program Executes....." << endl;
					break;
				}

				clock++;
			}

			string str = "";
			int z = clock;
			if( clock > 3000 )
				z = 2999;
			int k;
			for( int i = 1 ; i <= inscnt ; i++ )
			{
				if( s[i][clock] == "  WB   " )
				{
					k = i;
					break;
				}
			}
			if( inscnt > 1000 )
				k = 999;

			int loweri = 1;
			int upperi = 10;
			int lowerc = 0;
			int upperc = 0;
			while( true )
			{
				for( int i = 0 ; i <= z ; )
				{
					if( s[loweri][i] != "  IF   " )
						i++;
					else
					{
						lowerc = i;
						break;
					}
				}

				int u = upperi;

				if( s[upperi][0].substr(0,2) == "sq" )
				{
					if( s[upperi-1][0].substr(0,2) == "sq" )
					{
						u = u - 2;
					}
					else
						u = u - 1;
				}

				for( int i = 0 ; i <= z ; )
				{
					if( s[u][i] != "  WB   " )
						i++;
					else
					{
						upperc = i;
						break;
					}
				}
				if( upperc > 999 || lowerc > 999 )
					break;
				str = str +  "                           ";

				for( int j = lowerc ; j <= upperc ; j++ )
				{
					str = str + "C" + to_string(j);
					if( j < 10 )
					{
						str = str + "      ";
					}
					else if( j < 100 )
					{
						str = str + "     ";
					}
					else
					{
						str = str + "    ";
					}
				}
				str = str + "\n";

				for( int i = loweri ; i <= upperi ; i++ )
		 		{
					str = str + "I" + to_string(i);
					if( i < 10 )
					{
						str = str + " ";
					}
					str = str + "|";
					str = str + s[i][0] + "\t|";
					for( int j = lowerc ; j <= upperc ; j++ )
					{
						str = str + s[i][j] + "|";
					}
					str = str + "\n-------------------------";
					for( int j = lowerc ; j <= upperc ; j++ )
					{
						str = str + "--------";
					}
					str =  str + "\n";
				}
	
				if( upperi == k )
					break;

				loweri = upperi + 1;
				if( (upperi+10) < k )
					upperi = upperi + 10;
				else
					upperi = k;
			}

			TotalNoOfInstructionsExecuted = NoOfConditionalBranches + NoOfUnconditionalBranches + NoOfLoadStores + NoOfArithmetic + 1;
			int NoOfConditionalBranchesTaken = NoOfBranchesTaken - NoOfUnconditionalBranches;
			int TotalNoOfStalls = NoOfStallsBecauseOfDataDependency + NoOfStallsBecauseOfBranches;
			string filename;
			string file = "";
			if( !pipeline )
			{
				filename = "Unpipelined.txt";
				file = file + "UnPipelined Execution\n\n";
			}
			else if( pipeline and !OperandForwarding )
			{
				filename = "PipelineWithoutOperandForwarding.txt";
				file = file + "Pipelined Exeecution Without Operand Forwarding\n\n";
			}
			else if( pipeline and OperandForwarding )
			{
				filename = "PipelineWithOperandForwarding.txt";
				file = file + "Pipelined Execution With Operand Forwarding\n\n";
			}

			file = file + "Instruction Count Statistics :\n";
			file = file + "Dynamic Count of Arithmetic Instructions Executed = " + to_string( NoOfArithmetic ) + "\n";
			file = file + "Dynamic Count of Memory Accessing Instructions Executed = " + to_string( NoOfLoadStores ) + "\n";
			file = file + "Dynamic Count of Unconditional Branch Instructions Executed = " + to_string( NoOfUnconditionalBranches ) + "\n";
			file = file + "Dynamic Count of Conditional Branch Instructions Executed = " + to_string( NoOfConditionalBranches ) + "\n";
			file = file + "Total Number of Instructions Executed = " + to_string( TotalNoOfInstructionsExecuted ) + "\n\n";
			file = file + "Stalls :\n";
			file = file + "Number of Conditional Branches Taken = " + to_string( NoOfConditionalBranchesTaken ) + "\n";
			file = file + "Total Number of Branches Taken = " + to_string( NoOfBranchesTaken ) + "\n";
			file = file + "Total Number of Stalls because of Branch Instructions = " + to_string( NoOfStallsBecauseOfBranches ) + "\n";
			file = file + "Total Number of Stalls because of Data Dependency between successive Instructions = " + to_string( NoOfStallsBecauseOfDataDependency ) + "\n";
			file = file + "Total Number of Stall Cycles = " + to_string( TotalNoOfStalls ) + "\n\n";
			file = file + "Total Number of Clock Cycles taken to Execute the Entire Program = " + to_string( clock ) + "\n\n";
			string tfile = file;
			file = file + str;

			ofstream out(filename);
			out << file;
			out.close();
			mem.WriteToFile();	
			return tfile;
		}

		struct IF_ID_RF InstructionFetch( struct WB_IF wi , int inscnt )
		{
			struct IF_ID_RF ii;
			ii.inscnt = inscnt;
			ii.pc = PC;

			if( wi.OperateOrNot == false )
			{
				ii.OperateOrNot = false;
				ii.instruction = 0;
			}
			else
			{
				if( pipeline == true )
				{
					ii.instruction = mem.getWord( PC );
					PC = PC + 4;
					ii.OperateOrNot = true;
				}
				else
				{
					if( wi.completedOrNot == false )
					{
						ii.OperateOrNot = false;
						ii.instruction = 0;
					}
					else
					{
						ii.instruction = mem.getWord( PC );
						PC = PC + 4;
						ii.OperateOrNot = true;
					}
				}
			}
			return ii;
		}

		struct ID_RF_EX InstructionDecodeAndRegisterFetch( struct IF_ID_RF ii , bool of )
		{
			struct ID_RF_EX ie;
			ie.inscnt = ii.inscnt;
			ie.pc = ii.pc;
			ie.of = of;

			string ins = "";
			if( ii.OperateOrNot == false )
			{
				ie.OperateOrNot = false;
			}
			else
			{
				ie.OperateOrNot = true;
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
					int ad2 = n & inst;
					
					
					n = n << 8;
					int ad1 = n & inst;
					ad1 = ad1 >> 8;

					n = n << 8;
					int add = n & inst;
					add = add >> 16;

					ie.destination = add;

					ins = ins + "add " + "$" + to_string(add) + ",$" + to_string(ad1) + ",$" + to_string(ad2);
					ie.ins = ins;

					if( reg.getRegisterIsUsedOrNot( ad2 ) == true )
					{
						if( !of )
						{
							ie.stallOrNot = true;
							ie.OperateOrNot = false;
							return ie;
						}
						else
						{
							ie.value2FetchedOrNot = false;
							ie.value2 = ad2;
						}
					}
					else
						ie.value2 = reg.getRegister( ad2 );

					if( reg.getRegisterIsUsedOrNot( ad1 ) == true )
					{
						if( !of )
						{
							ie.stallOrNot = true;
							ie.OperateOrNot = false;
							return ie;
						}
						else
						{
							ie.value1FetchedOrNot = false;
							ie.value1 = ad1;
						}
					}
					else
						ie.value1 = reg.getRegister( ad1 );

					reg.setRegisterIsUsedOrNot( add , true );
					ie.regSetToUsed = add;
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
					ie.destination = add;

					ins = ins + "sub " + "$" + to_string(add) + ",$" + to_string(ad1) + ",$" + to_string(ad2);
					ie.ins = ins;
					if( reg.getRegisterIsUsedOrNot( ad2 ) == true )
					{
						if( !of )
						{
							ie.stallOrNot = true;
							ie.OperateOrNot = false;
							return ie;
						}
						else
						{
							ie.value2FetchedOrNot = false;
							ie.value2 = ad2;
						}
					}
					else
						ie.value2 = reg.getRegister( ad2 );
					
					if( reg.getRegisterIsUsedOrNot( ad1 ) == true )
					{
						if( !of )
						{
							ie.stallOrNot = true;
							ie.OperateOrNot = false;
							return ie;
						}
						else
						{
							ie.value1FetchedOrNot = false;
							ie.value1 = ad1;
						}
					}
					else
						ie.value1 = reg.getRegister( ad1 );

					reg.setRegisterIsUsedOrNot( add , true );
					ie.regSetToUsed = add;
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
						ie.value1 = n & inst;
						ie.value2 = 0;
						n = 31;
						n = n << 16;
						int ad = n & inst;
						ad = ad >> 16;
						reg.setRegisterIsUsedOrNot( ad , true );
						ie.regSetToUsed = ad;
						ie.destination = ad;
						ins = ins + "lw " + "$" + to_string(ad) + "," + to_string(ie.value1) + "\t";
					}
					else
					{
						int n = 255;
						int ad2 = n & inst;
						
							
						n = n << 8;
						int ad1 = n & inst;
						ad1 = ad1 >> 8;
						
						ie.value1 = ad1;
	
						n = n << 8;
						int add = n & inst;
						add = add >> 16;
						ie.destination = add;

						ins = ins + "lw " + "$" + to_string(add) + "," + to_string(ad1) + "($" + to_string(ad2) + ")\t";
						ie.ins = ins;

						if( reg.getRegisterIsUsedOrNot( ad2 ) == true )
						{
							if( !of )
							{
								ie.stallOrNot = true;
								ie.OperateOrNot = false;
								return ie;
							}
							else
							{
								ie.value2FetchedOrNot = false;
								ie.value2 = ad2;
							}
						}
						else
							ie.value2 = reg.getRegister( ad2 );

						reg.setRegisterIsUsedOrNot( add , true );
						ie.regSetToUsed = add;
					}
				}
				else if( c == 4 )
				{
					int n = 255;
					int ad2 = n & inst;
					
					
					n = n << 8;
					int ad1 = n & inst;
					ad1 = ad1 >> 8;
					ie.value1 = ad1;

					n = n << 8;
					int add = n & inst;
					add = add >> 16;
					
					ins = ins + "sw " + "$" + to_string(add) + "," + to_string(ad1) + "($" + to_string(ad2) + ")\t";
					ie.ins = ins;

					if( reg.getRegisterIsUsedOrNot( ad2 ) == true )
					{
						if( !of )
						{
							ie.stallOrNot = true;
							ie.OperateOrNot = false;
							return ie;
						}
						else
						{
							ie.value2FetchedOrNot = false;
							ie.value2 = ad2;
						}
					}
					else
						ie.value2 = reg.getRegister( ad2 );
					
					if( reg.getRegisterIsUsedOrNot( add ) == true )
					{
						if( !of )
						{
							ie.stallOrNot = true;
							ie.OperateOrNot = false;
							return ie;
						}
						else
						{
							ie.destinationFetchedOrNot = false;
							ie.destination = add;
						}
					}
					else
						ie.destination = reg.getRegister( add );
				}
				else if( c == 5 || c == 6 || c == 10 )
				{
					int n = 16383;
					int add = n & inst;
					ie.destination = add;

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
				        ie.ins = ins;	
					
					if( reg.getRegisterIsUsedOrNot( ad2 ) == true )
					{
						if( !of )
						{
							ie.stallOrNot = true;
							ie.OperateOrNot = false;
							return ie;
						}
						else
						{
							ie.value2FetchedOrNot = false;
							ie.value2 = ad2;
						}
					}
					else
						ie.value2 = reg.getRegister( ad2 );
					
					if( reg.getRegisterIsUsedOrNot( ad1 ) == true )
					{
						if( !of )
						{
							ie.stallOrNot = true;
							ie.OperateOrNot = false;
							return ie;
						}
						else
						{
							ie.value1FetchedOrNot = false;
							ie.value1 = ad1;
						}
					}
					else
						ie.value1 = reg.getRegister( ad1 );
				}
				else if( c == 7 )
				{
					int n = 16383;
					int ad = n & inst;

					ie.destination = ad;
					ie.value1 = 0;
					ie.value2 = 0;
					ins = ins + "j " + to_string(ad) + "\t";
				}
				else if( c == 8 )
				{
					int n = 16383;
					ie.value1 = n & inst;
					ie.value2 = 0;
					n = 31;
					n = n << 16;
					int ad = n & inst;
					ad = ad >> 16;
					reg.setRegisterIsUsedOrNot( ad , true );
					ie.regSetToUsed = ad;
					ie.destination = ad;
					ins = ins + "li $" + to_string(ad) + "," + to_string(ie.value1) + "\t";
				}
				else if( c == 9 )
				{
					int n = 16383;
					ie.value1 = n & inst;
					ie.value2 = 0;
					n = 31;
					n = n << 16;
					int ad = n & inst;
					ad = ad >> 16;
					reg.setRegisterIsUsedOrNot( ad , true );
					ie.regSetToUsed = ad;
					ie.destination = ad;
					ins = ins + "la $" + to_string(ad) + "," + to_string( ie.value1 ) + "\t";
				}
				else if( c == 0 )
				{
					ie.value1 = 0;
					ie.value2 = 0;
					ie.destination = 0;	
					ins = "NULL\t";
				}

			}
			ie.ins = ins;
			return ie;
		}

		struct EX_MEM Execute( struct ID_RF_EX ie , struct EX_MEM emb , struct MEM_WB mw , struct WB_IF wi )
		{
			struct EX_MEM em;
			em.instruction = ie.instruction;
			em.inscnt = ie.inscnt;
			em.pc = ie.pc;

			if( ie.OperateOrNot == false )
			{
				em.OperateOrNot = false;
			}
			else
			{
				em.OperateOrNot = true;
				em.stallOrNot = false;

				if( ie.of == true and pipeline )
				{
					if( ie.value1FetchedOrNot == false )
					{
						if( wi.completedOrNot and wi.reg == ie.value1 )
						{
							ie.value1 = wi.value;
							ie.value1FetchedOrNot = true;
						}

						if( emb.performOrNot == false )
						{
							if( ie.value1 == emb.address )
							{
								ie.value1 = emb.value;
								ie.value1FetchedOrNot = true;
							}
						}

						if( ie.value1FetchedOrNot == false )
						{
							if( ie.value1 == mw.reg )
							{
								ie.value1 = mw.value;
							}
							else
							{
								em.stallOrNot = true;
								em.OperateOrNot = false;
								return em;
							}
						}
					}

					if( ie.value2FetchedOrNot == false )
					{
						if( wi.completedOrNot and wi.reg == ie.value2 )
						{
							ie.value2 = wi.value;
							ie.value2FetchedOrNot = true;
						}

						if( emb.performOrNot == false )
						{
							if( ie.value2 == emb.address )
							{
								ie.value2 = emb.value;
								ie.value2FetchedOrNot = true;
							}
						}

						if( ie.value2FetchedOrNot == false )
						{
							if( ie.value2 == mw.reg )
							{
								ie.value2 = mw.value;
							}
							else
							{
								em.stallOrNot = true;
								em.OperateOrNot = false;
								return em;
							}
						}
					}

					if( ie.destinationFetchedOrNot == false )
					{
						if( wi.completedOrNot and wi.reg == ie.destination )
						{
							ie.destination = wi.value;
							ie.destinationFetchedOrNot = true;
						}

						if( emb.performOrNot == false )
						{
							if( ie.destination == emb.address )
							{
								ie.destination = emb.value;
								ie.destinationFetchedOrNot = true;
							}
						}

						if( ie.destinationFetchedOrNot == false )
						{
							if( ie.destination == mw.reg )
							{
								ie.destination = mw.value;
							}
							else
							{
								em.stallOrNot = true;
								em.OperateOrNot = false;
								return em;
							}
						}
					}
				}

				int inst = ie.instruction;
				if( inst == 0 )
				{
					em.value = 0;
					em.address = 32;
					em.performOrNot = false;
				}
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
					{
						PC = ie.destination;
						em.branchOrNot = true;
					}
					em.value = -1;
					em.address = 32;
					em.performOrNot = false;
				}
				else if( inst == 6 )
				{
					if( ie.value1 != ie.value2 )
					{
						PC = ie.destination;
						em.branchOrNot = true;
					}
					em.value = -1;
					em.address = 32;
					em.performOrNot = false;
				}
				else if( inst == 7 )
				{
					em.performOrNot = false;
					em.address = 32;
					em.value = -1;
					PC = ie.destination;
					em.branchOrNot = true;
				}
				else if( inst == 8 )
				{
					em.value = ie.value1 + ie.value2;
					em.address = ie.destination;
					em.performOrNot = false;
				}
				else if( inst == 9 )
				{
					em.value = ie.value1 + ie.value2;
					em.address = ie.destination;
					em.performOrNot = false;
				}
				else if( inst == 10 )
				{
					if( ie.value1 < ie.value2 )
					{
						PC = ie.destination;
						em.branchOrNot = true;
					}
					em.value = -1;
					em.address = 32;
					em.performOrNot = false;
				}
			}
			return em;
		}

		struct MEM_WB MemoryWriteAndRead( struct EX_MEM em )
		{
			struct MEM_WB mw;
			mw.instruction = em.instruction;
			mw.inscnt = em.inscnt;
			mw.pc = em.pc;

			if( em.OperateOrNot == false )
			{
				mw.OperateOrNot = false;
			}
			else
			{
				mw.stallOrNot = false;
				mw.OperateOrNot = true;
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
						mw.reg = em.address;
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
			wi.instruction = mw.instruction;
			wi.inscnt = mw.inscnt;
			wi.pc = mw.pc;
			wi.completedOrNot = false;

			wi.stop = false;

			if( mw.OperateOrNot == true )
			{
				if( mw.performOrNot == false )
				{
					wi.completedOrNot = true;
					wi.mem = mw.reg;
				}
				else
				{
					wi.completedOrNot = true;
					if( mw.reg == 32 )
					{
						if( mw.value == 0 )
						{
							wi.completedOrNot = true;
							wi.stop = true;
						}
					}
					else
					{
						reg.setRegister( mw.reg , mw.value );
						reg.setRegisterIsUsedOrNot( mw.reg , false );
					}
					wi.reg = mw.reg;
				}
			}
			wi.value = mw.value; 
			return wi;
		}
		void Print( int regi , int pc , int memo )
		{
			cout << "\e[36m" << "\tRegisters\t\tUser Memory\t\tInstruction Memory" << "\e[0m" << endl;
			cout << "\e[35m" << "Num\tName\tValue\t     Address   value\t\tPC\t\tInstruction" << "\e[0m" << endl;
			int r = 0;
			int p = 1;
			int m = 0;
			for( ; r < 32 || p < size || mem.getWord( m ) != 0 ; r++,p++,m+=4 )
			{
				if( r < 32 )
				{
					if( r == regi )
					{
						cout << "\e[33m" << "$" << r << "\t$" << reg.registerName[r] << "\t" << reg.getRegister(r) << "\e[0m" << "\t\t" ;
					}
					else
					{
						cout << "$" << r << "\t$" << reg.registerName[r] << "\t" << reg.getRegister(r) << "\t\t";
					}
				}
				else
				{
					cout << "\t\t\t\t" ;
				}
				
				if( mem.getWord( m ) != 0 )
				{
					if( memo == m )
					{
						cout << "\e[31m" << m << "\t" << mem.getWord(m) << "\e[0m" << "\t\t";
					}
					else
					{
						cout << m << "\t" << mem.getWord(m) << "\t\t";
					}
				}
				else
				{
					cout << "\t\t\t";
				}
				
				if( p <= size )
				{
					if( p == (pc-8200)/4 + 1 )
					{
						cout << "\e[35m" << (4*(p-1))+8200 << "\t\t" << Instructions[p] << "\e[0m" << endl;;
					}
					else
					{
						cout << (4*(p-1))+8200 << "\t\t" << Instructions[p] << endl;
					}
				}
				else
				{
					cout << endl;
				}

			}
		}
};
