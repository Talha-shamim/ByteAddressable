struct IF_ID_RF
{
	int instruction = 0;
	bool stallOrNot = false;
};

struct ID_RF_EX
{
	int instruction = 0;
	int value1 = 0;
	int value2 = 0;
	int destination = 0;
	bool stallOrNot = false;
};

struct EX_MEM
{
	bool performOrNot = false;
	bool readOrWrite = false;
	int value = 0;
	int address = 0;
	bool stallOrNot = false;
};

struct MEM_WB
{
	bool performOrNot = false;
	int value = 0;
	int reg = 0;
	bool stallOrNot = false;
};

struct WB_IF
{
	bool completedOrNot = false;
};
