#include"Register.h"
#include"Memory.h"
Memory mem;
Register reg;
struct IF_ID_RF
{
	int instruction = 0;
	bool stallOrNot = false;
	bool OperateOrNot = true;
	int inscnt = 0;
	bool branchPredictedTakenOrNot = false;
	int pc = -1;
};

struct ID_RF_EX
{
	int instruction = 0;
	int value1 = 0;
	int value2 = 0;
	int destination = 0;
	bool value1FetchedOrNot = true;
	bool value2FetchedOrNot = true;
	bool destinationFetchedOrNot = true;
	bool of = false;
	bool stallOrNot = false;
	bool OperateOrNot = true;
	int inscnt = 0;
	int regSetToUsed = -1;
	bool branchResolvedOrNot = false;
	bool branchTakenOrNot = false;
	bool branchPredictedTakenOrNot = false;
	bool branchPredictedCorrectlyOrNot = false;
	bool firstStallOrNot = true;
	string ins = "";
	int pc = -1;
};

struct EX_MEM
{
	int instruction = 0;
	bool performOrNot = false;
	bool readOrWrite = false;
	int value = 0;
	int address = 0;
	bool stallOrNot = false;
	bool OperateOrNot = true;
	int inscnt = 0;
	bool branchOrNot = false;
	bool earlyBranchOrNot = false;
	bool earlyResolutionOrNot = false;
	bool branchPredictedTakenOrNot = false;
	bool branchPredictedCorrectlyOrNot = false;
	int pc = -1;
};

struct MEM_WB
{
	int instruction = 0;
	bool performOrNot = false;
	int value = 0;
	int reg = 0;
	bool stallOrNot = false;
	bool OperateOrNot = true;
	int inscnt = 0;
	int stalls = 0;
	int pc = -1;
};

struct WB_IF
{
	int instruction = 0;
	bool completedOrNot = false;
	bool stop = false;
	int inscnt = 0;
	bool OperateOrNot = true;
	int reg = -1;
	int value = 0;
	int pc = -1;
	int mem = -1;
};
