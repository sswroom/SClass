#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/RandomMT19937.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Data/Sort/BubbleSort.h"
#include "Data/Sort/InsertionSort.h"
#include "Data/Sort/InsertionSort_C.h"
#include "Data/Sort/PigeonholeSort.h"
#include "Data/Sort/QuickBubbleSort.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "Manage/HiResClock.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"

#define NUM_RECORD 10000000

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Manage::HiResClock *clk;
	Data::RandomMT19937 *rand;
	Data::Sort::ArtificialQuickSort *sort;
	Text::StringBuilderUTF8 sb;
	IO::ConsoleWriter console;
	UnsafeArray<UnsafeArray<WChar>> array1;
	UnsafeArray<UnsafeArray<WChar>> array2;
	WChar **array2p;
	UnsafeArray<WChar> arrBuff;

	UInt32 seed = 0;
	UIntOS argc;
	UnsafeArray<UnsafeArray<UTF8Char>> argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc >= 2)
	{
		Text::StrToUInt32(argv[1], seed);
	}

	NEW_CLASS(rand, Data::RandomMT19937(seed));
	NEW_CLASS(clk, Manage::HiResClock());
	NEW_CLASS(sort, Data::Sort::ArtificialQuickSort());
	array1 = MemAllocAArr(UnsafeArray<WChar>, NUM_RECORD);
	array2 = MemAllocAArr(UnsafeArray<WChar>, NUM_RECORD);
	array2p = MemAllocA(WChar *, NUM_RECORD);
	arrBuff = MemAlloc(WChar, NUM_RECORD * 11);

	clk->Start();
	Double t1;
    UIntOS i;
    UIntOS numberOfRec;
	UnsafeArray<WChar> cptr = arrBuff;
	Bool valid;
	i = 0;
	while (i < NUM_RECORD)
	{
		array1[i] = cptr;
		cptr = Text::StrInt32(cptr, rand->NextInt32()) + 1;
		i++;
	}
	t1 = clk->GetTimeDiff();
	sb.ClearStr();
	sb.Append(CSTR("Gen Random N = "));
	sb.AppendI32((Int32)NUM_RECORD);
	sb.Append(CSTR(", seed = "));
	sb.AppendU32(seed);
	sb.Append(CSTR(", time = "));
	sb.AppendDouble(t1);
	sb.Append(CSTR("s"));
	console.WriteLine(sb.ToCString());

	clk->Start();
	MemCopyANC(array2.Ptr(), array1.Ptr(), NUM_RECORD * sizeof(UnsafeArray<WChar>));
	t1 = clk->GetTimeDiff();
	sb.ClearStr();
	sb.Append(CSTR("Memory copy time = "));
	sb.AppendDouble(t1);
	sb.Append(CSTR("s"));
	console.WriteLine(sb.ToCString());

	numberOfRec = NUM_RECORD;
	MemCopyANC(array2.Ptr(), array1.Ptr(), numberOfRec * sizeof(UnsafeArray<WChar>));
	clk->Start();
	sort->SortStrW(array2, 0, (IntOS)numberOfRec - 1);
	t1 = clk->GetTimeDiff();
	sb.ClearStr();
	sb.Append(CSTR("AQuickSort "));
	sb.AppendUIntOS(Sync::ThreadUtil::GetThreadCnt());
	sb.Append(CSTR(" Thread N = "));
	sb.AppendUIntOS(numberOfRec);
	sb.Append(CSTR(", Sort time = "));
	sb.AppendDouble(t1);
	sb.Append(CSTR("s"));
	console.WriteLine(sb.ToCString());
	console.Write(CSTR("First 100 values: "));
	i = 0;
	while (i < 100)
	{
		sb.ClearStr();
		sb.AppendW(array2[i]);
		sb.Append(CSTR(" "));
		console.Write(sb.ToCString());
		i++;
	}
	console.WriteLine();
	console.Write(CSTR("Last 100 values: "));
	i = NUM_RECORD - 100;
	while (i < NUM_RECORD)
	{
		sb.ClearStr();
		sb.AppendW(array2[i]);
		sb.Append(CSTR(" "));
		console.Write(sb.ToCString());
		i++;
	}
	console.WriteLine();

	numberOfRec = 128;
	while (numberOfRec <= NUM_RECORD)
	{
		MemCopyANC(array2p, array1.Ptr(), numberOfRec * sizeof(UnsafeArray<WChar>));
		clk->Start();
		ArtificialQuickSort_SortStrW(array2p, 0, (IntOS)numberOfRec - 1);
		t1 = clk->GetTimeDiff();
		sb.ClearStr();
		sb.Append(CSTR("AQuickSort 1 Thread N = "));
		sb.AppendUIntOS(numberOfRec);
		sb.Append(CSTR(", Sort time = "));
		sb.AppendDouble(t1);
		sb.Append(CSTR("s"));
		console.Write(sb.ToCString());
		valid = true;
		i = 0;
		while (i < numberOfRec - 1)
		{
			if (Text::StrCompare(array2p[i], array2p[i + 1]) > 0)
			{
				valid = false;
				break;
			}
			i++;
		}
		if (valid)
		{
			console.WriteLine(CSTR(", Result sorted"));
		}
		else
		{
			console.WriteLine(CSTR(", Result wrong"));
		}
		if (t1 >= 1.0)
		{
			break;
		}
		numberOfRec = numberOfRec << 1;
	}

	numberOfRec = 128;
	while (numberOfRec <= NUM_RECORD)
	{
		MemCopyANC(array2.Ptr(), array1.Ptr(), numberOfRec * sizeof(UnsafeArray<WChar>));
		clk->Start();
		sort->SortStrW(array2, 0, (IntOS)numberOfRec - 1);
		t1 = clk->GetTimeDiff();
		sb.ClearStr();
		sb.Append(CSTR("AQuickSort "));
		sb.AppendUIntOS(Sync::ThreadUtil::GetThreadCnt());
		sb.Append(CSTR(" Thread N = "));
		sb.AppendUIntOS(numberOfRec);
		sb.Append(CSTR(", Sort time = "));
		sb.AppendDouble(t1);
		sb.Append(CSTR("s"));
		console.Write(sb.ToCString());
		valid = true;
		i = 0;
		while (i < numberOfRec - 1)
		{
			if (Text::StrCompare(UnsafeArray<const WChar>(array2[i]), UnsafeArray<const WChar>(array2[i + 1])) > 0)
			{
				valid = false;
				break;
			}
			i++;
		}
		if (valid)
		{
			console.WriteLine(CSTR(", Result sorted"));
		}
		else
		{
			console.WriteLine(CSTR(", Result wrong"));
		}
		if (t1 >= 1.0)
		{
			break;
		}
		numberOfRec = numberOfRec << 1;
	}

	numberOfRec = 16;
	while (numberOfRec <= NUM_RECORD)
	{
		MemCopyANC(array2p, array1.Ptr(), numberOfRec * sizeof(UnsafeArray<WChar>));
		clk->Start();
		InsertionSort_SortStrW(array2p, 0, (IntOS)numberOfRec - 1);
		t1 = clk->GetTimeDiff();
		sb.ClearStr();
		sb.Append(CSTR("InsertSort 1 Thread N = "));
		sb.AppendUIntOS(numberOfRec);
		sb.Append(CSTR(", Sort time = "));
		sb.AppendDouble(t1);
		sb.Append(CSTR("s"));
		console.Write(sb.ToCString());
		valid = true;
		i = 0;
		while (i < numberOfRec - 1)
		{
			if (Text::StrCompare(array2p[i], array2p[i + 1]) > 0)
			{
				valid = false;
				break;
			}
			i++;
		}
		if (valid)
		{
			console.WriteLine(CSTR(", Result sorted"));
		}
		else
		{
			console.WriteLine(CSTR(", Result wrong"));
		}
		if (t1 >= 1.0)
		{
			break;
		}
		numberOfRec = numberOfRec << 1;
	}

	numberOfRec = 16;
	while (numberOfRec <= NUM_RECORD)
	{
		MemCopyANC(array2p, array1.Ptr(), numberOfRec * sizeof(UnsafeArray<WChar>));
		clk->Start();
		InsertionSort_SortBStrW(array2p, 0, (IntOS)numberOfRec - 1);
		t1 = clk->GetTimeDiff();
		sb.ClearStr();
		sb.Append(CSTR("InsertSortB 1 Thread N = "));
		sb.AppendUIntOS(numberOfRec);
		sb.Append(CSTR(", Sort time = "));
		sb.AppendDouble(t1);
		sb.Append(CSTR("s"));
		console.Write(sb.ToCString());
		valid = true;
		i = 0;
		while (i < numberOfRec - 1)
		{
			if (Text::StrCompare(array2p[i], array2p[i + 1]) > 0)
			{
				valid = false;
				break;
			}
			i++;
		}
		if (valid)
		{
			console.WriteLine(CSTR(", Result sorted"));
		}
		else
		{
			console.WriteLine(CSTR(", Result wrong"));
		}
		if (t1 >= 1.0)
		{
			break;
		}
		numberOfRec = numberOfRec << 1;
	}

	DEL_CLASS(sort);
	DEL_CLASS(clk);
	DEL_CLASS(rand);
	MemFreeArr(arrBuff);
	MemFreeAArr(array1);
	MemFreeAArr(array2);
	MemFreeA(array2p);
	return 0;
}

