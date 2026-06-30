#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Data/RandomOS.h"
#include "IO/Console.h"
#include "Manage/HiResClock.h"

#define ARRCOUNT 1048576

Int32 compCnt;
class TestVal : public Data::Comparable
{
public:
	Int32 val;

public:
	TestVal(Int32 val)
	{
		this->val = val;
	}
	~TestVal()
	{
	}

	virtual IntOS CompareTo(NN<Data::Comparable> obj) const
	{
		Int32 i = NN<TestVal>::ConvertFrom(obj)->val;
		compCnt++;
		if (val > i)
			return 1;
		else if (val < i)
			return -1;
		else
			return 0;
	}
};

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<Manage::HiResClock> clk;
	NN<Data::RandomOS> rand;
	UnsafeArray<NN<Data::Comparable>> arr;
	Int32 j;
	Int32 i;
	Double totalSpd = 0;

	NEW_CLASSNN(clk, Manage::HiResClock());
	NEW_CLASSNN(rand, Data::RandomOS());
	arr = MemAllocArr(NN<Data::Comparable>, ARRCOUNT);
	compCnt = 0;
	j = 10;
	while (j-- > 0)
	{
		i = ARRCOUNT;
		while (i-- > 0)
		{
			NEW_CLASSNN(arr[i], TestVal((rand->NextInt32() << 15) | rand->NextInt32()));
		}
		Double spd;
		clk->Start();
		Data::Sort::ArtificialQuickSort::SortCmpO(arr, 0, ARRCOUNT - 1);
		spd = clk->GetTimeDiff();
		totalSpd += spd;
		i = ARRCOUNT;
		while (i-- > 1)
		{
			if (arr[i - 1]->CompareTo(arr[i]) > 0)
			{
				printf("Error in sorting\r\n");
				break;
			}
		}
		printf("Time: %lf\n", spd);

		i = ARRCOUNT;
		while (i-- > 0)
		{
			arr[i].Delete();
		}
	}
	printf("Total Time: %lf\n", totalSpd);
	MemFreeArr(arr);
	IO::Console::GetChar();
	rand.Delete();
	clk.Delete();
	return 0;
}