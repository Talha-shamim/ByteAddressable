#include<iostream>
#include<stdio.h>
#include<fstream>
#include"BranchPredictor.h"
using namespace std;
BranchPredictor bpredict;

string s[3000];
unsigned char table[3000][30000];

class Processor
{
	private:
		int clock = 1;
		int PC = 8192;
		bool pipeline;
		bool OperandForwarding;
		bool EarlyBranchResolution;
		bool stepwiseExecution;
		bool predictor;
		bool cacheOrNot;
		bool ExecutionTable = false;
		Cache* cache;
	public:
		Processor( Cache* c )
		{
			cache = c;
		}

		void Process( bool pipe , bool oper , bool early , bool step , bool ca , bool bp , bool extable)
		{
			pipeline = pipe;
			OperandForwarding = oper;
			EarlyBranchResolution = early;
			stepwiseExecution = step;
			if( stepwiseExecution )
				pipeline = false;
			predictor = bp;
			cacheOrNot = ca;
			ExecutionTable = extable;

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

			for( int i = 0 ; i < 3000 ; i++ )
					s[i] = "NOP    \t";
			int n = 16383;
			int ad = n & mem.getWord( PC );
			PC = ad;

			int g = 1;
			int inscnt = 1;
			bool stop = false;
			string typesOfInstructions[17] = { "add" , "sub" , "lw" , "sw" , "beq" , "bne" , "j" , "li" , "la" , "blt" , "addi" , "mul" , "and" , "or" , "bgt" , "jr" , "jal" };
			int array[17] = {0};
			int NoOfConditionalBranches = 0;
			int NoOfUnconditionalBranches = 0;
			int NoOfLoadStores = 0;
			int NoOfLoads = 0;
			int NoOfStores = 0;
			int NoOfArithmetic = 0;
			int NoOfBranchesTaken = 0;
			int NoOfjrInstructions = 0;
			int NoOfCorrectBranchPredictions = 0;
			int NoOfEarlyBranchResolutions = 0;
			int NoOfBranchesTakenThroughEarlyBranchResolution = 0;
			int NoOfStallsBecauseOfBranches = 0;
			int NoOfStallsBecauseOfDataDependency = 0;
			int NoOfMemoryStalls = 0;
			int NoOfConditionalBranchesTaken = 0;
			int TotalNoOfInstructionsExecuted = 0;
			int cnt = 0;
			int cps = 1;
			if( cacheOrNot )
				cps = cache->l1time;
			while( true )
			{
				
				if( stepwiseExecution )
				{
					if( g == 1 )
					{
						string s;
						cout << "Enter 'e' and Press 'Enter' to Execute the First Instruction" << endl;
						cin >> s;
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
						cin >> s;
						if( s == "e" )
						{
							g++;
							stop = false;
						}
						else if( s == "b" )
						{
							stop = false;
							break;
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
	
				wiafter = WriteBack( mwbefore );
				ID_RF_EX tempp = InstructionDecodeAndRegisterFetch( iibefore , OperandForwarding );
				
				if( tempp.OperateOrNot == false and emafter.stallOrNot == true )
				{
				}
				else
					ieafter = tempp;
				if( tempp.OperateOrNot )
				{
					if( tempp.branchResolvedOrNot )
					{
						if( tempp.branchTakenOrNot )
							iiafter.OperateOrNot = false;
					}
					s[ieafter.inscnt] = ieafter.ins;
				}

				emafter = Execute( iebefore , embefore , mwbefore , wibefore );
	
				if( predictor and emafter.stallOrNot == false and pipeline )
				{
					if( emafter.instruction == 5 or emafter.instruction == 6 or emafter.instruction == 10 or emafter.instruction == 15 )
					{
						if( emafter.branchPredictedCorrectlyOrNot )
							NoOfCorrectBranchPredictions++;
					}
				}
				if( emafter.branchOrNot and pipeline)
				{
					NoOfBranchesTaken++;
					if( emafter.instruction == 5 or emafter.instruction == 6 or emafter.instruction == 10 or emafter.instruction == 15 )
						NoOfConditionalBranchesTaken++;
				}
				if( emafter.branchOrNot and emafter.earlyResolutionOrNot == false and pipeline == true  and emafter.stallOrNot == false )
				{
					if( ieafter.stallOrNot == true )
						NoOfStallsBecauseOfDataDependency--;
					ieafter.OperateOrNot = false;
					ieafter.stallOrNot = false;
					if( ieafter.regSetToUsed != -1 )
						reg.setRegisterIsUsedOrNot( ieafter.regSetToUsed , false );
					
					string sq = s[emafter.inscnt+1];
					s[emafter.inscnt+1] = "sq-" + sq;
					sq = mem.GetInstruction( mem.getWord( iiafter.pc + 8 ) );
					s[emafter.inscnt+2] = "sq-" + sq;

					iiafter.OperateOrNot = false;
					iiafter.stallOrNot = false;
					table[ieafter.inscnt][clock] = 2;
					if( cacheOrNot )
						NoOfStallsBecauseOfBranches = NoOfStallsBecauseOfBranches + 2*cache->l1time;
					else
						NoOfStallsBecauseOfBranches = NoOfStallsBecauseOfBranches + 2;
				}
				else if( emafter.earlyResolutionOrNot and pipeline and emafter.stallOrNot == false )
				{
					if( emafter.branchOrNot )
					{
						string sq = mem.GetInstruction( mem.getWord( iiafter.pc + 4 ) );
						s[emafter.inscnt+1] = "sq-" + sq;
						
						if( cacheOrNot )
							NoOfStallsBecauseOfBranches += cache->l1time;
						else
							NoOfStallsBecauseOfBranches++;
						NoOfBranchesTakenThroughEarlyBranchResolution++;
						NoOfEarlyBranchResolutions++;
					}
					else
					{
						NoOfEarlyBranchResolutions++;
					}
				}
				else if( pipeline and emafter.earlyResolutionOrNot )
				{
					NoOfEarlyBranchResolutions++;
				}

				mwafter = MemoryWriteAndRead( embefore );
				
				if( temp.OperateOrNot )
				{
					for( int i = 0 ; i < cps ; i++ )
						table[temp.inscnt][i+clock] = 1;
				}
				if( tempp.stallOrNot )
				{
					bool b = false;
					for( int i = clock-1 ; i >= 0 ; i++ )
					{
						if( table[tempp.inscnt][i] == 2 )
						{
							b = true;
							break;
						}
						else if( table[tempp.inscnt][i] == 1 )
							break;
					}
					if( b )
					{
						for( int i = 0 ; i < cps ; i++ )
							table[tempp.inscnt][i+clock] = 6;
					}
					else
					{
						for( int i = 0 ; i < cps ; i++ )
							table[tempp.inscnt][i+clock] = 2;
					}
					NoOfStallsBecauseOfDataDependency += cps;
				}
				if( tempp.OperateOrNot  and !tempp.stallOrNot )
				{
					for( int i = 0 ; i < cps ; i++ )
						table[tempp.inscnt][i+clock] = 2;
				}
				if( emafter.stallOrNot )
				{
					for( int i = 0 ; i < cps ; i++ )
						table[emafter.inscnt][i+clock] = 6;
					NoOfStallsBecauseOfDataDependency += cps;
				}
				if( emafter.OperateOrNot and !emafter.stallOrNot )
				{
					for( int i = 0 ; i < cps ; i++ )
						table[emafter.inscnt][i+clock] = 3;
					bool b = false;
					for( int i = clock-1 ; i >= 0 ; i-- )
					{
						if( table[emafter.inscnt][i] == 6 )
						{
							b = true;
							break;
						}
						else if( table[emafter.inscnt][i] == 3 )
						{
							break;
						}
						else if( table[emafter.inscnt][i] == 0 )
						{
							continue;
						}
						else
							break;
					}
					if( b )
					{
						for( int i = 0 ; i < cps ; i++ )
						{
							table[emafter.inscnt+1][clock+i] = 2;
							table[emafter.inscnt+2][clock+i] = 1;
						}
					}
				}
				if( mwafter.OperateOrNot )
				{
					for( int i = 0 ; i < cps ; i++ )	
						table[mwafter.inscnt][i+clock] = 4;
				}
				if( wiafter.completedOrNot )
				{
					for( int i = 0 ; i < cps ; i++ )
						table[wiafter.inscnt][i+clock] = 5;
					if( wiafter.instruction == 1 || wiafter.instruction == 2 || wiafter.instruction == 8 || wiafter.instruction == 9 || wiafter.instruction == 11 || wiafter.instruction == 12 || wiafter.instruction == 13 || wiafter.instruction == 14 )
						NoOfArithmetic++;
					else if( wiafter.instruction == 3 )
					{
						NoOfLoads++;
						NoOfLoadStores++;
					}
					else if( wiafter.instruction == 4 )
					{
						NoOfStores++;
						NoOfLoadStores++;
					}
					else if( wiafter.instruction == 5 || wiafter.instruction == 6 || wiafter.instruction == 10 || wiafter.instruction == 15 )
						NoOfConditionalBranches++;
					else if( wiafter.instruction == 7 || wiafter.instruction == 16 || wiafter.instruction == 17 )
					{
						NoOfUnconditionalBranches++;
						if( wiafter.instruction == 16 )
							NoOfjrInstructions++;
					}
					array[wiafter.instruction-1]++;
				}
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
					iibefore.OperateOrNot = false;
					for( int i = 0 ; i < cps ; i++ )
						table[temp.inscnt][i+clock] = 0;
					for( int i = 0 ; i < cps ; i++ )
						table[tempp.inscnt][i+clock] = 0;
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
					clock = clock + cps-1;
					cout << "clock = " << clock << endl;
					cout << "Program Executed" << endl;
					break;
				}
				NoOfMemoryStalls += mwafter.stalls;
				for( int i = clock+cps ; i <= clock + mwafter.stalls+cps-1 ; i++ )
				       table[mwafter.inscnt][i] = 4;
				if( cacheOrNot )
				{
					clock  = clock + cache->l1time + mwafter.stalls;
					for( int i = 0 ; i < cache->l1time + mwafter.stalls and !buffer.empty() ; i++ )
					{
						if( buffer.front().stalls != 1 )
						{
							buffer.front().stalls = buffer.front().stalls - 1;
						}
						else
						{
							for( int j = 0 ; j < cache->getBlockSize() ; j++ )
							{
								mem.writeByte( buffer.front().b[j] , buffer.front().tag + j );
							}
							buffer.pop();
						}
					}
				}
				else
				{
					clock = clock + 1;
				}
			}	

			string str = "";
			if( ExecutionTable )
			{
				int z = clock;
				if( clock > 30000 )
					z = 29999;
				int k;
				for( int i = 1 ; i <= inscnt ; i++ )
				{
					if( table[i][clock] == 5 )
					{
						k = i;
						break;
					}
				}
				if( inscnt > 3000 )
					k = 2999;
	
				int loweri = 1;
				int upperi = 10;
				if( k < 10 )
					upperi = k;
				int lowerc = 0;
				int upperc = 0;
				int e = k/100;
				int x = 0;
				cout << "Please wait while the file is being written" << endl;
				while( upperi <= k and loweri <= k and upperc <= z and lowerc <= z )
				{
					for( int i = 0 ; i <= z ; )
					{
						if( table[loweri][i] != 1 )
							i++;
						else
						{
							lowerc = i;
							break;
						}
					}
	
					int u = upperi;
	
					if( s[upperi].substr(0,2) == "sq" )
					{
						if( s[upperi-1].substr(0,2) == "sq" )
						{
							u = u - 2;
						}
						else
							u = u - 1;
					}
	
					for( int i = 0 ; i <= z ; )
					{
						if( table[u][i] != 5 )
							i++;
						else if( table[u][i] == 5 and table[u][i+1] == 5 )
							i++;
						else
						{
							upperc = i;
							break;
						}
					}
					str = str +  "                                   ";
	
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
						else if( j < 1000 )
						{
							str = str + "    ";
						}
						else if( j < 10000 )
						{
							str = str + "   ";
						}
						else
						{
							str = str + "  ";
						}
					}
					str = str + "\n";
	
					for( int i = loweri ; i <= upperi ; i++ )
			 		{
						if( e > 0 )
						{
							if( i % e == 0  and x != 100 )
							{
								cout << "\r";
								x++;	
								for( int t = 0 ; t < x ; t++ )
									cout << "#";
								cout << " -" << x << "%";
								fflush(stdout);
							}
						}
						str = str + "I" + to_string(i);
						if( i < 10 )
						{
							str = str + " ";
						}
						str = str + "|";
						str = str + s[i] + "\t|";
						for( int j = lowerc ; j <= upperc ; j++ )
						{
							if( table[i][j] == 0 )
							{
								str = str + "       |";
							}
							else if( table[i][j] == 1 )
							{
								str = str + "   IF  |";
							}
							else if( table[i][j] == 2 )
							{
								str = str + " ID/RF |";
							}
							else if( table[i][j] == 3 )
							{
								str = str + "   EX  |";
							}
							else if( table[i][j] == 4 )
							{
								str = str + "  MEM  |";
							}
							else if( table[i][j] == 5 )
							{
								str = str + "   WB  |";
							}
							else if( table[i][j] == 6 )
							{
								str = str + " stall |";
							}
						}
						str = str + "\n---------------------------------";
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
			}
			
			cout << "\nPC = " << PC << endl;
			string file = "";
			TotalNoOfInstructionsExecuted = NoOfConditionalBranches + NoOfUnconditionalBranches + NoOfLoadStores + NoOfArithmetic + 1;
			file = file + "\t\t\t\t\t\t\t\t\tBYTE ADDRESSABLE\n\n";
			file = file + "Type of Execution :\n";
			file = file + "\tPipelined Or Unpipelined \t\t:";
			if( pipeline )
			{
				file = file + " Pipelined\n";
				file = file + "\tOperand Forwarding Enabled Or Not \t: ";
				if( OperandForwarding )
				{
					file = file + "Enabled\n";
				}
				else
					file = file + "Disabled\n";
				file = file + "\tEarly Branch Resolution Enabled Or Not \t: ";
				if( EarlyBranchResolution )
				{
					file = file + "Enabled\n";
				}
				else
					file = file + "Disabled\n";
				file = file + "\tBranch Predictor Enabled Or Not \t: ";
				if( predictor )
					file = file + "Enabled\n";
				else
					file = file + "Disabled\n";
			}

			else
			{
				file = file + " UnPipelined\n";
			}

			file = file + "\tCache Enabled Or Not \t:";
			if( cacheOrNot )
			{
				file = file + " Enabled\n";
				file = file + "\t\tSize of L1 Cache\t = " + to_string( cache->L1size ) + "\n";
				file = file + "\t\tSize of L2 Cache\t = " + to_string( cache->L2size ) + "\n";
				file = file + "\t\tBlock Size\t\t = " + to_string( cache->BlockSize ) + "\n";
				file = file + "\t\tAssociativity\t\t = " + to_string( cache->Associativity ) + "\n";
				file = file + "\t\tNo of offset bits in L1\t = " + to_string( cache->NoOfOffsetL1bits ) + "\n";
				file = file + "\t\tNo of index bits in L1\t = " + to_string( cache->NoOfIndexL1bits ) + "\n";
				file = file + "\t\tNo of offset bits in L2\t = " + to_string( cache->NoOfOffsetL2bits ) + "\n";
				file = file + "\t\tNo of index bits in L2\t = " + to_string( cache->NoOfIndexL2bits ) + "\n";
				file = file + "\t\tL1 Access Time\t\t = " + to_string( cache->l1time ) + "\n";
				file = file + "\t\tL2 Access Time\t\t = " + to_string( cache->l2time ) + "\n";
				file = file + "\t\tBuffer Access Time\t = " + to_string( cache->buffertime ) + "\n";
				file = file + "\t\tMemory Access Time\t = " + to_string( cache->memorytime ) + "\n";
				file = file + "\t\tReplaceMent Policy\t = ";
				if( cache->policy == 1 )
					file = file + "LRU\n";
				else if( cache->policy == 2 )
					file = file + "LFU\n";
				else if( cache->policy == 3 )
					file = file + "FIFO\n";
				else if( cache->policy == 4 )
					file = file + "RANDOM\n";
				else if( cache->policy == 5 )
					file = file + "LRU-LFU-FIFO\n\n";
			}
			else
			{
				file = file + " Disabled\n\n";
			}

			file = file + "Instruction Count Statistics :\n";
			file = file + "\tInstruction Type:  ";
			for( int i = 0 ; i < 17 ; i++ )
				file = file + typesOfInstructions[i] + "\t";
			file = file + "\n\tCount\t\t:  ";
			for( int i = 0 ; i < 17 ; i++ )
				file = file + to_string(array[i]) + "\t";

			file = file + "\n\n\tTotal Number of Arithmetic Instructions Executed\t\t :  " + to_string( NoOfArithmetic ) + "\n";
			file = file + "\tTotal Number of Memory Instructions Executed\t\t\t :  " + to_string( NoOfLoadStores ) + "\n";
			file = file + "\tTotal Number of Unconditional Branch Instructions Executed\t :  " + to_string( NoOfUnconditionalBranches ) + "\n";
			file = file + "\tTotal Number of Conditional Branch Instructions Executed\t :  " + to_string( NoOfConditionalBranches ) + "\n";
			file = file + "\tTotal Number of InstructionS Executed\t\t\t\t :  " + to_string( TotalNoOfInstructionsExecuted ) + "\n";
			file = file + "\tNOTE : Total number of instructions executed includes one NULL terminate instruction also\n";

			if( !pipeline )
			{
				file = file + "\n\tClock cycle time per Execution Stage : ";
				if( cacheOrNot )
					file = file + to_string( cache->l1time ) + "\n";
				else
					file = file + "1\n";
			}
			else
			{
				file = file + "\n\tClock cycle time per Execution Stage : ";
				if( cacheOrNot )
					file = file + to_string( cache->l1time ) + "\n";
				else
					file = file + "1\n";

				file = file + "\nStalls :\n";
			        if( predictor )
				{
					file = file + "\tTotal Number of Correct Branch Predictions :  " + to_string( NoOfCorrectBranchPredictions ) + "\n";
					file = file + "\tNOTE : Branch Predictions are made only for Conditional Branches, Unconditional Branches like j and jal are not considered under Correct Branch Predictions even Though the target address is computed at IF stage( so no stalls )\n";
				}
				else
					file = file + "\tTotal Number of Conditional Branches taken :  " + to_string( NoOfConditionalBranchesTaken ) + "\n";
				
				file = file + "\tTotal Number of Branches taken :  " + to_string( NoOfBranchesTaken ) + "\n";
				if( EarlyBranchResolution )
					file = file + "\tTotal Number of Early Branch Resolutions( out of branches taken ) :  " + to_string( NoOfBranchesTakenThroughEarlyBranchResolution ) + "\n";

				file = file + "\n\tTotal Number of Stalls because of Branch Instructions :  " + to_string( NoOfStallsBecauseOfBranches ) + "\n";
				file = file + "\tTotal Number of stalls because of Data Dependency between successive Instructions\t :  " + to_string( NoOfStallsBecauseOfDataDependency ) + "\n";
			}

			if( cacheOrNot )
			{	
				int storel1misses = NoOfStores - storel1hits;
				int storel2misses = storel1misses - storel2hits;
				int storebuffermisses = storel2misses - storebufferhits;
				file = file + "\n\tCache Hits and Misses for Loads : \n";
				file = file + "\t\tL1 cache hits\t = " +  to_string( L1hits ) + "\n";
				file = file + "\t\tL1 cache misses\t = " + to_string( L1misses ) + "\n";
				file = file + "\t\tL2 cache hits\t = " + to_string( L2hits ) + "\n";
				file = file + "\t\tL2 cache misses\t = " + to_string( L2misses ) + "\n";
				file = file + "\t\tBuffer hits\t = " + to_string( bufferhits ) + "\n";
				file = file + "\t\tBuffer misses\t = " + to_string( buffermisses ) + "\n";
				file = file + "\t\tMain Memory Accesses     = " + to_string( buffermisses ) + "\n";
				
				file = file + "\n\t\tStalls because of L2 hits  \t = " + to_string( L2hits*cache->l2time ) + "\n";
				file = file + "\t\tStalls because of Buffer hits  \t = " + to_string( bufferhits*( cache->l2time + cache->buffertime ) ) + "\n";
				file = file + "\t\tStalls because of Memory Accesses = " + to_string( buffermisses*( cache->l2time + cache->buffertime + cache->memorytime ) ) + "\n";

				file = file + "\n\tTotal Number of Stalls Because of Loads : " + to_string( L2hits*cache->l2time + bufferhits*( cache->l2time + cache->buffertime ) + buffermisses*( cache->l2time + cache->buffertime + cache->memorytime ) ) + "\n"; 
	
				file = file + "\n\tCache Hits and Misses for Stores : \n";
				file = file + "\t\tL1 cache hits\t = " +  to_string( storel1hits ) + "\n";
				file = file + "\t\tL1 cache misses\t = " + to_string( storel1misses ) + "\n";
				file = file + "\t\tL2 cache hits\t = " + to_string( storel2hits ) + "\n";
				file = file + "\t\tL2 cache misses\t = " + to_string( storel2misses ) + "\n";
				file = file + "\t\tBuffer hits\t = " + to_string( storebufferhits ) + "\n";
				file = file + "\t\tBuffer misses\t = " + to_string( storebuffermisses ) + "\n";
				file = file + "\t\tMain Memory Accesses    = " + to_string( storebuffermisses ) + "\n";


				
				file = file + "\n\t\tStalls because of L2 hits  \t = " + to_string( storel2hits*cache->l2time ) + "\n";
				file = file + "\t\tStalls because of Buffer hits  \t = " + to_string( storebufferhits*( cache->l2time + cache->buffertime ) ) + "\n";
				file = file + "\t\tStalls because of Memory Accesses = " + to_string( storebuffermisses*( cache->l2time + cache->buffertime + cache->memorytime ) ) + "\n";

				file = file + "\n\tTotal Number of Stalls Because of Stores : " + to_string( storel2hits*cache->l2time + storebufferhits*(cache->l2time + cache->buffertime ) + storebuffermisses*( cache->l2time + cache->buffertime + cache->memorytime ) ) + "\n";
				file = file + "\n\n\tTotal Number of Stalls Because of Memory : " + to_string( NoOfMemoryStalls ) + "\n"; 
			}	
				
			file = file + "\n\n\t\t\t\tTotal Number of Clock Cycles taken to Execute the Entire Program\t : " + to_string( clock ) + "\n\n\n\n";
			
			string tfile = file;
			cout << tfile << endl;
			file = file + str;

			ofstream out("output.txt");
			out << file;
			out.close();
		}

		struct IF_ID_RF InstructionFetch( struct WB_IF wi , int inscnt )
		{
			struct IF_ID_RF ii;
			ii.inscnt = inscnt;

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
					ii.pc = PC;
					if( predictor == false )
						PC = PC + 4;
					else
					{
						PC = bpredict.getPC( PC );
						if( PC != ii.pc + 4 )
							ii.branchPredictedTakenOrNot = true;
					}

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
						ii.pc = PC;
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
			ie.branchPredictedTakenOrNot = ii.branchPredictedTakenOrNot;

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

				if( c == 1 or c == 2 or c == 12 or c == 13 or c == 14 )
				{
					if( c == 1 )
						ins = ins + "add ";
					else if( c == 2 )
						ins = ins + "sub ";
					else if( c == 12 )
						ins = ins + "mul ";
					else if( c == 13 )
						ins = ins + "and ";
					else if( c == 14 )
						ins = ins + "or ";

					int n = 255;
					int ad2 = n & inst;
					
					
					n = n << 8;
					int ad1 = n & inst;
					ad1 = ad1 >> 8;

					n = n << 8;
					int add = n & inst;
					add = add >> 16;

					ie.destination = add; ins = ins + "$" + to_string(add) + ",$" + to_string(ad1) + ",$" + to_string(ad2);
				       	int len = 20-ins.length();
					for( int i = 0 ; i < len ; i++ )
						ins = ins + " ";
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
						ins = ins + "lw " + "$" + to_string(ad) + "," + to_string(ie.value1);
						int len = 20-ins.length();
						for( int i = 0 ; i < len ; i++ )
							ins = ins + " ";
						ie.ins = ins;
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

						ins = ins + "lw " + "$" + to_string(add) + "," + to_string(ad1) + "($" + to_string(ad2) + ")";
						int len = 20-ins.length();
						for( int i = 0 ; i < len ; i++ )
							ins = ins + " ";

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
					
					ins = ins + "sw " + "$" + to_string(add) + "," + to_string(ad1) + "($" + to_string(ad2) + ")";
					int len = 20-ins.length();
					for( int i = 0 ; i < len ; i++ )
						ins = ins + " ";
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
				else if( c == 5 || c == 6 || c == 10 || c == 15 )
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
					else if( c == 10 )
						ins = ins + "blt";
					else if( c == 15 )
						ins = ins + "bgt";

					ins = ins + " $" + to_string(ad1) + ",$" + to_string(ad2) + "," + to_string(add);
					int len = 20-ins.length();
					for( int i = 0 ; i < len ; i++ )
						ins = ins + " ";
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

					if( EarlyBranchResolution and pipeline )
					{
						if( ie.value1FetchedOrNot and ie.value2FetchedOrNot )
						{
							if( c == 5 )
							{
								if( ie.value1 == ie.value2 )
								{
									if( predictor )
									{
										if( ii.branchPredictedTakenOrNot == false )
										{
											PC = ie.destination;
											ie.branchPredictedCorrectlyOrNot = false;
											ie.branchTakenOrNot = true;
										}
										else
										{
											ie.branchPredictedCorrectlyOrNot = true;
											ie.branchTakenOrNot = false;
										}
									}
									else
									{
										PC = ie.destination;
										ie.branchTakenOrNot = true;
									}
								}
								else
								{
									if( predictor )
									{
										if( ii.branchPredictedTakenOrNot == true )
										{
											PC = ie.pc+4;
											ie.branchPredictedCorrectlyOrNot = false;
											ie.branchTakenOrNot = true;
										}
										else
										{
											ie.branchPredictedCorrectlyOrNot = true;
											ie.branchTakenOrNot = false;
										}
									}
								}
							}
							else if( c == 6 )
							{
								if( ie.value1 != ie.value2 )
								{
									if( predictor )
									{
										if( ii.branchPredictedTakenOrNot == false )
										{
											PC = ie.destination;
											ie.branchPredictedCorrectlyOrNot = false;
											ie.branchTakenOrNot = true;
										}
										else
										{
											ie.branchPredictedCorrectlyOrNot = true;
											ie.branchTakenOrNot = false;
										}
									}
									else
									{
										PC = ie.destination;
										ie.branchTakenOrNot = true;
									}
								}
								else
								{
									if( predictor )
									{
										if( ii.branchPredictedTakenOrNot == true )
										{
											PC = ie.pc+4;
											ie.branchPredictedCorrectlyOrNot = false;
											ie.branchTakenOrNot = true;
										}
										else
										{
											ie.branchPredictedCorrectlyOrNot = true;
											ie.branchTakenOrNot = false;
										}
									}
								}

							}
							else if( c == 10 )
							{
								if( ie.value1 < ie.value2 )
								{
									if( predictor )
									{
										if( ii.branchPredictedTakenOrNot == false )
										{
											PC = ie.destination;
											ie.branchPredictedCorrectlyOrNot = false;
											ie.branchTakenOrNot = true;
										}
										else
										{
											ie.branchPredictedCorrectlyOrNot = true;
											ie.branchTakenOrNot = false;
										}
									}
									else
									{
										PC = ie.destination;
										ie.branchTakenOrNot = true;
									}
								}
								else
								{
									if( predictor )
									{
										if( ii.branchPredictedTakenOrNot == true )
										{
											PC = ie.pc+4;
											ie.branchPredictedCorrectlyOrNot = false;
											ie.branchTakenOrNot = true;
										}
										else
										{
											ie.branchPredictedCorrectlyOrNot = true;
											ie.branchTakenOrNot = false;
										}
									}
								}

							}
							else if( c == 15 )
							{
								if( ie.value1 > ie.value2 )
								{
									if( predictor )
									{
										if( ii.branchPredictedTakenOrNot == false )
										{
											PC = ie.destination;
											ie.branchPredictedCorrectlyOrNot = false;
											ie.branchTakenOrNot = true;
										}
										else
										{
											ie.branchPredictedCorrectlyOrNot = true;
											ie.branchTakenOrNot = false;
										}
									}
									else
									{
										PC = ie.destination;
										ie.branchTakenOrNot = true;
									}
								}
								else
								{
									if( predictor )
									{
										if( ii.branchPredictedTakenOrNot == true )
										{
											PC = ie.pc+4;
											ie.branchPredictedCorrectlyOrNot = false;
											ie.branchTakenOrNot = true;
										}
										else
										{
											ie.branchPredictedCorrectlyOrNot = true;
											ie.branchTakenOrNot = false;
										}
									}
								}
							}
							ie.branchResolvedOrNot = true;
						}
						else
						{
							ie.branchResolvedOrNot = false;
						}
					}	
				}
				else if( c == 7 or c == 17 or c == 16 )
				{
					int n = 16383;
					int ad = n & inst;
					ie.value1 = 0;
					ie.value2 = 0;

					if( c == 7 )
						ins = ins + "j ";
					else if( c == 17 )
					{
						ins = ins + "jal ";
						reg.setRegisterIsUsedOrNot( 31 , true );
						ie.regSetToUsed = 31;
					}
					else if( c == 16 )
						ins = ins + "jr $";

					ins = ins + to_string(ad);
					int len = 20-ins.length();
					for( int i = 0 ; i < len ; i++ )
						ins = ins + " ";

					if( c != 16 )
						ie.destination = ad;
					else
					{
						if( reg.getRegisterIsUsedOrNot( ad ) == true )
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
								ie.destination = ad;
							}
						}
						else
							ie.destination = reg.getRegister( ad );
					}

					if( EarlyBranchResolution and pipeline )
					{
						if( ie.value1FetchedOrNot and ie.value2FetchedOrNot and ie.destinationFetchedOrNot )
						{
							if( predictor )
							{
								if( ii.branchPredictedTakenOrNot )
								{
									ie.branchTakenOrNot = false;
									ie.branchResolvedOrNot = true;
									ie.branchPredictedCorrectlyOrNot = true;
								}
							}
							else
							{
								PC = ie.destination;
								ie.branchResolvedOrNot = true;
								ie.branchTakenOrNot = true;
								ie.branchPredictedCorrectlyOrNot = false;
							}
						}
						else
						{
							ie.branchResolvedOrNot = false;
						}
					}
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
					ins = ins + "li $" + to_string(ad) + "," + to_string(ie.value1);
					int len = 20-ins.length();
					for( int i = 0 ; i < len ; i++ )
						ins = ins + " ";
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
					ins = ins + "la $" + to_string(ad) + "," + to_string( ie.value1 );
					int len = 20-ins.length();
					for( int i = 0 ; i < len ; i++ )
						ins = ins + " ";
				}
				else if( c == 11 )
				{
					int n = 16383;
					ie.value2= n & inst;

					n = 31;
					n = n << 14;
					int ad1 = n & inst;
					ad1 = ad1 >> 14;

					n = n << 5;
					int add = n & inst;
					add = add >> 19;

					ins = ins + "addi $" + to_string(add) + ",$" + to_string(ad1) + ", " + to_string( ie.value2 );
					int len = 20-ins.length();
					for( int i = 0 ; i < len ; i++ )
						ins = ins + " ";
					ie.ins = ins;

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
					ie.destination = add;
				}
				else if( c == 0 )
				{
					ie.value1 = 0;
					ie.value2 = 0;
					ie.destination = 0;	
					ins = "NULL\t";
					int len = 20-ins.length();
					for( int i = 0 ; i < len ; i++ )
						ins = ins + " ";
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
			em.branchPredictedTakenOrNot = ie.branchPredictedTakenOrNot;
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
					if( ie.branchResolvedOrNot )
					{
						em.branchOrNot = ie.branchTakenOrNot;
						em.earlyBranchOrNot = true;
						em.earlyResolutionOrNot = true;
						em.branchPredictedCorrectlyOrNot = ie.branchPredictedCorrectlyOrNot;
					}
					else
					{
						if( ie.value1 == ie.value2 )
						{
							if( predictor )
							{
								if( em.branchPredictedTakenOrNot == false )
								{
									PC = ie.destination;
									em.branchPredictedCorrectlyOrNot = false;
									em.branchOrNot = true;
								}
								else
								{
									em.branchPredictedCorrectlyOrNot = true;
									em.branchOrNot = false;
								}
							}
							else
							{
								PC = ie.destination;
								em.branchOrNot = true;
							}
						}
						else
						{
							if( predictor )
							{
								if( em.branchPredictedTakenOrNot == true )
								{
									PC = ie.pc+4;
									em.branchPredictedCorrectlyOrNot = false;
									em.branchOrNot = true;
								}
								else
								{
									em.branchPredictedCorrectlyOrNot = true;
									em.branchOrNot = false;
								}
							}
						}
					}
					em.value = -1;
					em.address = 32;
					em.performOrNot = false;
				}
				else if( inst == 6 )
				{
					if( ie.branchResolvedOrNot )
					{
						em.branchOrNot = ie.branchTakenOrNot;
						em.earlyBranchOrNot = true;
						em.earlyResolutionOrNot = true;
						em.branchPredictedCorrectlyOrNot = ie.branchPredictedCorrectlyOrNot;
					}
					else
					{
						if( ie.value1 != ie.value2 )
						{
							if( predictor )
							{
								if( em.branchPredictedTakenOrNot == false )
								{
									PC = ie.destination;
									em.branchPredictedCorrectlyOrNot = false;
									em.branchOrNot = true;
								}
								else
								{
									em.branchPredictedCorrectlyOrNot = true;
									em.branchOrNot = false;
								}
							}
							else
							{
								PC = ie.destination;
								em.branchOrNot = true;
							}
						}
						else
						{
							if( predictor )
							{
								if( em.branchPredictedTakenOrNot == true )
								{
									PC = ie.pc+4;
									em.branchPredictedCorrectlyOrNot = false;
									em.branchOrNot = true;
								}
								else
								{
									em.branchPredictedCorrectlyOrNot = true;
									em.branchOrNot = false;
								}
							}
						}

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
					if( ie.branchResolvedOrNot )
					{
						em.branchOrNot = ie.branchTakenOrNot;
						em.earlyBranchOrNot = true;
						em.earlyResolutionOrNot = true;
						em.branchPredictedCorrectlyOrNot = ie.branchPredictedCorrectlyOrNot;
					}
					else
					{
						if( predictor )
						{
							if( em.branchPredictedTakenOrNot == false )
							{
								PC = ie.destination;
								em.branchPredictedCorrectlyOrNot = false;
								em.branchOrNot = true;
							}
							else
							{
								em.branchPredictedCorrectlyOrNot = true;
								em.branchOrNot = false;
							}
						}
						else
						{
							PC = ie.destination;
							em.branchOrNot = true;
						}
					}
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
					if( ie.branchResolvedOrNot )
					{
						em.branchOrNot = ie.branchTakenOrNot;
						em.earlyBranchOrNot = true;
						em.earlyResolutionOrNot = true;
						em.branchPredictedCorrectlyOrNot = ie.branchPredictedCorrectlyOrNot;
					}
					else
					{
						if( ie.value1 < ie.value2 )
						{
							if( predictor )
							{
								if( em.branchPredictedTakenOrNot == false )
								{
									PC = ie.destination;
									em.branchPredictedCorrectlyOrNot = false;
									em.branchOrNot = true;
								}
								else
								{
									em.branchPredictedCorrectlyOrNot = true;
									em.branchOrNot = false;
								}
							}
							else
							{
								PC = ie.destination;
								em.branchOrNot = true;
							}
						}
						else
						{
							if( predictor )
							{
								if( em.branchPredictedTakenOrNot == true )
								{
									PC = ie.pc+4;
									em.branchPredictedCorrectlyOrNot = false;
									em.branchOrNot = true;
								}
								else
								{
									em.branchPredictedCorrectlyOrNot = true;
									em.branchOrNot = false;
								}
							}
						}

					}

					em.value = -1;
					em.address = 32;
					em.performOrNot = false;
				}
				else if( inst == 11 )
				{
					em.value = ie.value1 + ie.value2;
					em.address = ie.destination;
					em.performOrNot = false;
				}
				else if( inst == 12 )
				{
					em.value = ie.value1 * ie.value2;
					em.address = ie.destination;
					em.performOrNot = false;
				}
				else if( inst == 13 )
				{
					em.value = ie.value1 & ie.value2;
					em.address = ie.destination;
					em.performOrNot = false;
				}
				else if( inst == 14 )
				{
					em.value = ie.value1 | ie.value2;
					em.address = ie.destination;
					em.performOrNot = false;
				}
				else if( inst == 15 )
				{
					if( ie.branchResolvedOrNot )
					{
						em.branchOrNot = ie.branchTakenOrNot;
						em.earlyBranchOrNot = true;
						em.earlyResolutionOrNot = true;
						em.branchPredictedCorrectlyOrNot = ie.branchPredictedCorrectlyOrNot;
					}
					else
					{
						if( ie.value1 > ie.value2 )
						{
							if( predictor )
							{
								if( em.branchPredictedTakenOrNot == false )
								{
									PC = ie.destination;
									em.branchPredictedCorrectlyOrNot = false;
									em.branchOrNot = true;
								}
								else
								{
									em.branchPredictedCorrectlyOrNot = true;
									em.branchOrNot = false;
								}
							}
							else
							{
								PC = ie.destination;
								em.branchOrNot = true;
							}
						}
						else
						{
							if( predictor )
							{
								if( em.branchPredictedTakenOrNot == true )
								{
									PC = ie.pc+4;
									em.branchPredictedCorrectlyOrNot = false;
									em.branchOrNot = true;
								}
								else
								{
									em.branchPredictedCorrectlyOrNot = true;
									em.branchOrNot = false;
								}
							}
						}

					}
					em.value = -1;
					em.address = 32;
					em.performOrNot = false;
				}
				else if( inst == 16 )
				{
					em.performOrNot = false;
					em.address = 32;
					em.value = -1;
					if( ie.branchResolvedOrNot )
					{
						em.branchOrNot = ie.branchTakenOrNot;
						em.earlyBranchOrNot = true;
						em.earlyResolutionOrNot = true;
					}
					else
					{
						if( predictor )
						{
							if( em.branchPredictedTakenOrNot == false )
							{
								PC = ie.destination;
								em.branchPredictedCorrectlyOrNot = false;
								em.branchOrNot = true;
							}
							else
							{
								em.branchPredictedCorrectlyOrNot = true;
								em.branchOrNot = false;
							}
						}
						else
						{
							PC = ie.destination;
							em.branchOrNot = true;
						}

					}
				}
				else if( inst == 17 )
				{
					em.performOrNot = false;
					em.address = 31;
					em.value = ie.pc+4;
					if( ie.branchResolvedOrNot )
					{
						em.branchOrNot = ie.branchTakenOrNot;
						em.earlyBranchOrNot = true;
						em.earlyResolutionOrNot = true;
						em.branchPredictedCorrectlyOrNot = ie.branchPredictedCorrectlyOrNot;
					}
					else
					{
						if( predictor )
						{
							if( em.branchPredictedTakenOrNot == false )
							{
								PC = ie.destination;
								em.branchPredictedCorrectlyOrNot = false;
								em.branchOrNot = true;
							}
							else
							{
								em.branchPredictedCorrectlyOrNot = true;
								em.branchOrNot = false;
							}
						}
						else
						{
							PC = ie.destination;
							em.branchOrNot = true;
						}

					}
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
						if( cacheOrNot)
						{
							StallNumber sn = cache->Load( em.address , 4 );
							mw.value = sn.value;
							mw.stalls = sn.stalls;
						}
						else
						{
							mw.stalls = 0;
							mw.value = mem.getWord( em.address );
						}
						mw.reg = em.value;
						mw.performOrNot = true;
					}
					else
					{
						if( cacheOrNot )
						{
							if( stepwiseExecution )
								mem.writeWord( em.value , em.address );
							mw.stalls = cache->Store( em.address , em.value , 4 ).stalls;
						}
						else
						{
							mw.stalls = 0;
							mem.writeWord( em.value , em.address );
						}
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
			wi.completedOrNot = false;
			wi.pc = mw.pc;

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
			cout << "\e[36m" << "\tRegisters\tUser Memory\tInstruction Memory" << "\e[0m" << endl;
			cout << "\e[35m" << "Num\tName   Value  Address   value\tPC\t\tInstruction" << "\e[0m" << endl;
			int r = 0;
			int p = 1;
			int m = 0;
			int count = 0;
			bool L1Completed = false;
			bool L2Completed = false;
			int mat = -1;
			int nat = -1;
			bool lrucomplete = false;
			bool lfucomplete = false;
			bool fifocomplete = false;
			bool printOrNot = false;
			bool finish = false;
			if( cacheOrNot )
			{
				if( cache->Associativity*cache->NoOfSetsL1 <= 16 and cache->Associativity*cache->NoOfSetsL2 <= 16 )
					printOrNot = true;
			}
			int blocks = 0;
			
			for( ; r < 32 || p < size || mem.getWord( m ) != 0 || !finish ; r++,p++,m+=4,count++ )
			{
				if( r < 32 )
				{
					if( r == regi )
					{
						cout << "\e[33m" << "$" << r << "\t$" << reg.registerName[r] << "\t" << reg.getRegister(r) << "\e[0m" << "\t" ;
					}
					else
					{
						cout << "$" << r << "\t$" << reg.registerName[r] << "\t" << reg.getRegister(r) << "\t";
					}
				}
				else
				{
					cout << "\t\t\t" ;
				}
				
				if( mem.getWord( m ) != 0 )
				{
					if( memo == m )
					{
						cout << "\e[31m" << m << "\t" << mem.getWord(m) << "\e[0m" << "\t";
					}
					else
					{
						cout << m << "\t" << mem.getWord(m) << "\t";
					}
				}
				else
				{
					cout << "\t\t";
				}
				
				if( p <= size )
				{
					if( p == (pc-8200)/4 + 1 )
					{
						cout << "\e[35m" << (4*(p-1))+8200 << "\t\t" << Instructions[p] << "\e[0m" << "\t";
					}
					else
					{
						cout << (4*(p-1))+8200 << "\t\t" << Instructions[p] << "\t";
					}
				}
				else
				{
					cout << "\t\t\t\t\t";
				}
				if( printOrNot )
				{
					if( nat == -1 )
					{
						cout << "\e[36m" << "L1 Cache" << "\e[0m" << endl;
						nat = 1;
					}
					else if( nat == 1 )
					{
						for( int i = 0 ; i < (cache->NoOfSetsL1) ; i++ )
						{
							cout << "\e[35m" << "Set " << i << "\e[0m" << "\t";
							for( int j = 1 ; j < cache->Associativity ; j++ )
								cout << "\t";
						}
						cout << endl;
						nat = 2;
					}
					else if( nat == 2 )
					{
						for( int j = 0 ; j < cache->NoOfSetsL1 ; j++ )
						{
							for( int i = 0 ; i < cache->Associativity ; i++ )
							{
								if( cache->L1Cache[j].b[i].tag != -1 )
									cout << "\e[33m" << "B=" << i << "\t" << "\e[0m";
								else
									cout << "B=" << i << "\t";	
							}
						}
						cout << endl;
						nat = 3;
					}
					else if( nat == 3 )
					{
						for( int j = 0 ; j < cache->NoOfSetsL1 ; j++ )
						{
							for( int i = 0 ; i < cache->Associativity ; i++ )
							{
								if( cache->L1Cache[j].b[i].tag != -1 )
									cout << "\e[33m" << "T=" << cache->L1Cache[j].b[i].tag << "\t" << "\e[0m";
								else
									cout << "T=" << cache->L1Cache[j].b[i].tag << "\t";
							}
						}
						nat = 4;
						if( cache->policy == 4 )
							nat = 7;
						cout << endl;
					}
					else if( !lrucomplete and nat == 4 and ( cache->policy == 1 or cache->policy == 5  ) )
					{
						for( int j = 0 ; j < cache->NoOfSetsL1 ; j++ )
						{
							for( int i = 0 ; i < cache->Associativity ; i++ )
							{
								if( cache->L1Cache[j].b[i].tag != -1 )
									cout << "\e[33m" << "R=" << cache->L1Cache[j].b[i].lru << "\t" << "\e[0m";
								else
									cout << "R=" << cache->L1Cache[j].b[i].lru << "\t";
							}
						}
						lrucomplete = true;
						if( cache->policy == 1 )
							nat = 7;
						cout << endl;
					}
					else if( !lfucomplete and nat == 4 and ( cache->policy == 2 or cache->policy == 5 ) )
					{
						for( int j = 0 ; j < cache->NoOfSetsL1 ; j++ )
						{
							for( int i = 0 ; i < cache->Associativity ; i++ )
							{
								if( cache->L1Cache[j].b[i].tag != -1 )
									cout << "\e[33m" << "F=" << cache->L1Cache[j].b[i].lfu << "\t" << "\e[0m";
								else
									cout << "F=" << cache->L1Cache[j].b[i].lfu << "\t";
							}
						}
						cout << endl;
						lfucomplete = true;
						if( cache->policy == 2 )
							nat = 7;
					}
					else if( !fifocomplete and nat == 4 and ( cache->policy == 3 or cache->policy == 5 ) )
					{
						for( int j = 0 ; j < cache->NoOfSetsL1 ; j++ )
						{
							for( int i = 0 ; i < cache->Associativity ; i++ )
							{
								if( cache->L1Cache[j].b[i].tag != -1 )
									cout << "\e[33m" << "O=" << cache->L1Cache[j].b[i].fifo << "\t" << "\e[0m";
								else
									cout << "O=" << cache->L1Cache[j].b[i].fifo << "\t";
							}
						}
						nat = 7;
						cout << endl;
						fifocomplete = true;
					}
					else if( L1Completed == false and nat == 7 )
					{
						for( int i = 0 ; i < cache->NoOfSetsL1 ; i++ )
						{
							for( int j = 0 ; j < cache->Associativity ; j++ )
							{
								if( cache->L1Cache[i].b[j].tag != -1 )
									cout << "\e[31m" << blocks << " " << (int)cache->L1Cache[i].b[j].b[blocks] << "\t" << "\e[0m";
								else
									cout << blocks << " " << (int)cache->L1Cache[i].b[j].b[blocks] << "\t";
							}
						}
						if( blocks == cache->BlockSize -1 )
							L1Completed = true;
						blocks++;
						cout << endl;
					}
					else if( L1Completed == true and L2Completed == false and mat == -1 )
					{
						cout << endl;
						lfucomplete = false;
						lrucomplete = false;
						fifocomplete = false;
						mat = 0;
					}
					else if( L1Completed == true and L2Completed == false and mat == 0 )
					{
						cout << "\e[36m" << "L2Cache" << "\e[0m" << endl;
						mat = 1;
						blocks = 0;
					}
					else if( L1Completed == true and L2Completed == false and mat == 1 )
					{
						for( int i = 0 ; i < cache->NoOfSetsL2 ; i++ )
						{
							cout << "\e[35m" << "Set " << i << "\t" << "\e[0m";
							for( int j = 1 ; j < cache->Associativity ; j++ )
								cout << "\t";
						}
						cout << endl;
						mat = 2;
					}	
					else if( L1Completed == true and L2Completed == false and mat == 2 )
					{
						for( int j = 0 ; j < cache->NoOfSetsL2 ; j++ )
						{
							for( int i = 0 ; i < cache->Associativity ; i++ )
							{
								if( cache->L2Cache[j].b[i].tag != -1 )
									cout << "\e[33m" << "B=" << i << "\t" << "\e[0m";
								else
									cout << "B=" << i << "\t";
							}
						}
						cout << endl;
						mat = 3;
					}
					else if( L1Completed == true and L2Completed == false and mat == 3 )
					{
						for( int j = 0 ; j < cache->NoOfSetsL2 ; j++ )
						{
							for( int i = 0 ; i < cache->Associativity ; i++ )
							{
								if( cache->L2Cache[j].b[i].tag != -1 )
									cout << "\e[33m" << "T=" << cache->L2Cache[j].b[i].tag << "\t" << "\e[0m";
								else
									cout << "T=" << cache->L2Cache[j].b[i].tag << "\t";
							}
						}
						cout << endl;
						mat = 4;
						if( cache->policy == 4 )
							mat = 7;
					}
					else if( !lrucomplete and L1Completed == true and L2Completed == false and mat == 4 and ( cache->policy == 1 or cache->policy == 5 ) )
					{
						for( int j = 0 ; j < cache->NoOfSetsL2 ; j++ )
						{
							for( int i = 0 ; i < cache->Associativity ; i++ )
							{
								if( cache->L2Cache[j].b[i].tag != -1 )
									cout << "\e[33m" << "R=" << cache->L2Cache[j].b[i].lru << "\t" << "\e[0m";
								else
									cout << "R=" << cache->L2Cache[j].b[i].lru << "\t";
							}
						}
						cout << endl;
						lrucomplete = true;
						if( cache->policy == 1 )
							mat = 7;
					}
					else if( !lfucomplete and L1Completed == true and L2Completed == false and mat == 4 and ( cache->policy == 2 or cache->policy == 5 ) )
					{
						for( int j = 0 ; j < cache->NoOfSetsL2 ; j++ )
						{
							for( int i = 0 ; i < cache->Associativity ; i++ )
							{
								if( cache->L2Cache[j].b[i].tag != -1 )
									cout << "\e[33m" << "F=" << cache->L2Cache[j].b[i].lfu << "\t" << "\e[0m";
								else
									cout << "F=" << cache->L2Cache[j].b[i].lfu << "\t";
							}
						}
						cout << endl;
						lfucomplete = true;
						if( cache->policy == 2 )
							mat = 7;
					}
					else if( !fifocomplete and L1Completed == true and L2Completed == false and mat == 4 and ( cache->policy == 3 or cache->policy == 5 ) )
					{
						for( int j = 0 ; j < cache->NoOfSetsL2 ; j++ )
						{
							for( int i = 0 ; i < cache->Associativity ; i++ )
							{
								if( cache->L2Cache[j].b[i].tag != -1 )
									cout << "\e[33m" << "O=" << cache->L2Cache[j].b[i].fifo << "\t" << "\e[0m";
								else
									cout << "O=" << cache->L2Cache[j].b[i].fifo << "\t";
							}
						}
						cout << endl;
						mat = 7;
						fifocomplete = true;
					}
					else if( L1Completed == true and L2Completed == false and mat == 7 )
					{
						for( int i = 0 ; i < cache->NoOfSetsL2 ; i++ )
						{
							for( int j = 0 ; j < cache->Associativity ; j++ )
							{
								if( cache->L2Cache[i].b[j].tag != -1 )
									cout << "\e[31m" << blocks << " " << (int)cache->L2Cache[i].b[j].b[blocks] << "\t" << "\e[0m";
								else
									cout << blocks << " " << (int)cache->L2Cache[i].b[j].b[blocks] << "\t";
							}
						}
						if( blocks == cache->BlockSize -1 )
							L2Completed = true;
						blocks++;
						cout << endl;
					}
					else if( L2Completed == true )
					{
						cout << endl;
						finish = true;
					}
				}
				else
				{
					finish = true;
					cout << endl;
				}
			}
			if( !printOrNot and cacheOrNot )
			{
				cout << "\e[36m" << "L1 Cache" << "\e[0m" << endl;
				for( int i = 0 ; i < (cache->NoOfSetsL1) ; i++ )
				{
					cout << "\e[35m" << "Set " << i << "\e[0m" << "\t";
					for( int j = 1 ; j < cache->Associativity ; j++ )		
						cout << "\t";
				}
					cout << endl;
				for( int j = 0 ; j < cache->NoOfSetsL1 ; j++ )
				{
					for( int i = 0 ; i < cache->Associativity ; i++ )
					{
						if( cache->L1Cache[j].b[i].tag != -1 )
							cout << "\e[33m" << "B=" << i << "\t" << "\e[0m";
						else
							cout << "B=" << i << "\t";	
					}
				}
				cout << endl;
				for( int j = 0 ; j < cache->NoOfSetsL1 ; j++ )
				{
					for( int i = 0 ; i < cache->Associativity ; i++ )
					{
						if( cache->L1Cache[j].b[i].tag != -1 )
							cout << "\e[33m" << "T=" << cache->L1Cache[j].b[i].tag << "\t" << "\e[0m";
						else
							cout << "T=" << cache->L1Cache[j].b[i].tag << "\t";
					}
				}
				cout << endl;
				if( cache->policy == 1 or cache->policy == 5 )
				{
					for( int j = 0 ; j < cache->NoOfSetsL1 ; j++ )
					{
						for( int i = 0 ; i < cache->Associativity ; i++ )
						{
							if( cache->L1Cache[j].b[i].tag != -1 )
								cout << "\e[33m" << "R=" << cache->L1Cache[j].b[i].lru << "\t" << "\e[0m";
							else
									cout << "R=" << cache->L1Cache[j].b[i].lru << "\t";
						}
					}
					cout << endl;
				}
				if( cache->policy == 2 or cache->policy == 5 )
				{
					for( int j = 0 ; j < cache->NoOfSetsL1 ; j++ )
					{
						for( int i = 0 ; i < cache->Associativity ; i++ )
							{
							if( cache->L1Cache[j].b[i].tag != -1 )
								cout << "\e[33m" << "F=" << cache->L1Cache[j].b[i].lfu << "\t" << "\e[0m";
							else
								cout << "F=" << cache->L1Cache[j].b[i].lfu << "\t";
						}
					}
					cout << endl;
				}
				if( cache->policy == 3 or cache->policy == 5 )
				{
					for( int j = 0 ; j < cache->NoOfSetsL1 ; j++ )
					{
						for( int i = 0 ; i < cache->Associativity ; i++ )
						{	
							if( cache->L1Cache[j].b[i].tag != -1 )
								cout << "\e[33m" << "O=" << cache->L1Cache[j].b[i].fifo << "\t" << "\e[0m";
							else
								cout << "O=" << cache->L1Cache[j].b[i].fifo << "\t";
						}
					}
					cout << endl;
				}
				for( int k = 0 ; k < cache->BlockSize ; k++ )
				{
					for( int i = 0 ; i < cache->NoOfSetsL1 ; i++ )
					{
						for( int j = 0 ; j < cache->Associativity ; j++ )
						{
							if( cache->L1Cache[i].b[j].tag != -1 )
								cout << "\e[31m" << k << " " << (int)cache->L1Cache[i].b[j].b[k] << "\t" << "\e[0m";
							else
								cout << k << " " << (int)cache->L1Cache[i].b[j].b[k] << "\t";
						}
					}
					cout << endl;
				}
				cout << "\e[36m" << "L2Cache" << "\e[0m" << endl;
				for( int i = 0 ; i < cache->NoOfSetsL2 ; i++ )
				{
					cout << "\e[35m" << "Set " << i << "\t" << "\e[0m";
					for( int j = 1 ; j < cache->Associativity ; j++ )
						cout << "\t";
				}
				cout << endl;
				for( int j = 0 ; j < cache->NoOfSetsL2 ; j++ )
				{
					for( int i = 0 ; i < cache->Associativity ; i++ )
					{
						if( cache->L2Cache[j].b[i].tag != -1 )
							cout << "\e[33m" << "B=" << i << "\t" << "\e[0m";
						else
							cout << "B=" << i << "\t";
					}
				}
				cout << endl;
				for( int j = 0 ; j < cache->NoOfSetsL2 ; j++ )
				{
					for( int i = 0 ; i < cache->Associativity ; i++ )
					{
						if( cache->L2Cache[j].b[i].tag != -1 )
							cout << "\e[33m" << "T=" << cache->L2Cache[j].b[i].tag << "\t" << "\e[0m";
						else
							cout << "T=" << cache->L2Cache[j].b[i].tag << "\t";
					}
				}
				cout << endl;
				if( cache->policy == 1 or cache->policy == 5 )
				{
					for( int j = 0 ; j < cache->NoOfSetsL2 ; j++ )
					{
						for( int i = 0 ; i < cache->Associativity ; i++ )
						{	
							if( cache->L2Cache[j].b[i].tag != -1 )
								cout << "\e[33m" << "R=" << cache->L2Cache[j].b[i].lru << "\t" << "\e[0m";
							else
								cout << "R=" << cache->L2Cache[j].b[i].lru << "\t";
						}
					}
					cout << endl;
				}
				if( cache->policy == 2 or cache->policy == 5 )
				{
					for( int j = 0 ; j < cache->NoOfSetsL2 ; j++ )
					{
						for( int i = 0 ; i < cache->Associativity ; i++ )
						{
							if( cache->L2Cache[j].b[i].tag != -1 )
								cout << "\e[33m" << "F=" << cache->L2Cache[j].b[i].lfu << "\t" << "\e[0m";
							else
								cout << "F=" << cache->L2Cache[j].b[i].lfu << "\t";
						}
					}
					cout << endl;
				}
				if( cache->policy == 3 or cache->policy == 5 )
				{
					for( int j = 0 ; j < cache->NoOfSetsL2 ; j++ )
					{
						for( int i = 0 ; i < cache->Associativity ; i++ )
						{
							if( cache->L2Cache[j].b[i].tag != -1 )
								cout << "\e[33m" << "O=" << cache->L2Cache[j].b[i].fifo << "\t" << "\e[0m";
							else
								cout << "O=" << cache->L2Cache[j].b[i].fifo << "\t";
						}
					}
					cout << endl;
				}
				for( int k = 0 ; k < cache->BlockSize ; k++ )
				{
					for( int i = 0 ; i < cache->NoOfSetsL2 ; i++ )
					{
						for( int j = 0 ; j < cache->Associativity ; j++ )
						{
							if( cache->L2Cache[i].b[j].tag != -1 )
								cout << "\e[31m" << k << " " << (int)cache->L2Cache[i].b[j].b[k] << "\t" << "\e[0m";
							else
								cout << k << " " << (int)cache->L2Cache[i].b[j].b[k] << "\t";
						}
					}
					cout << endl;
				}
			}
		}
};
