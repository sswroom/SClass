#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/RandomMT19937.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Data/Sort/BitonicSort.h"
#include "Data/Sort/BitonicSortC.h"
#include "Data/Sort/BubbleSort.h"
#include "Data/Sort/ByteCountingSort.h"
#include "Data/Sort/InsertionSort.h"
//#include "Data/Sort/PigeonholeSort.h"
//#include "Data/Sort/QuickBubbleSort.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "Manage/HiResClock.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

//#define NUM_RECORD 20000000
#define NUM_RECORD 100000000
//#define NUM_RECORD 50000000
//#define NUM_RECORD (2 << 26)


Int32 MyMain(Core::IProgControl *progCtrl)
{
	Manage::HiResClock *clk;
	Data::RandomMT19937 *rand;
	Data::Sort::ArtificialQuickSort *sort;
//	Data::Sort::PigeonholeSort *sort2;
	Data::Sort::BitonicSort *sort3;
	Text::StringBuilderUTF8 sb;
	IO::ConsoleWriter console;
	UInt32 *array1;
	UInt32 *array2;
	UOSInt recordCnt = NUM_RECORD;

	UInt32 seed = 0;
	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 2)
	{
		Text::StrToUInt32(argv[1], &seed);
	}

	NEW_CLASS(rand, Data::RandomMT19937(seed));
	NEW_CLASS(clk, Manage::HiResClock());
	NEW_CLASS(sort, Data::Sort::ArtificialQuickSort());
//	NEW_CLASS(sort2, Data::Sort::PigeonholeSort(0, NUM_RECORD - 1));
	NEW_CLASS(sort3, Data::Sort::BitonicSort());
	array1 = 0;
	array2 = 0;
	while (true)
	{
		if (array1)
		{
			MemFreeA(array1);
		}
		array1 = MemAllocA(UInt32, recordCnt);
		array2 = MemAllocA(UInt32, recordCnt);
		if (array1 && array2)
		{
			break;
		}
		recordCnt = recordCnt >> 1;
	}
	clk->Start();
	Double t1;
    UOSInt i;
    UOSInt NumberOfRec;
	Bool valid;
	i = 0;
	while (i < recordCnt)
	{
		array1[i] = (UInt32)rand->NextInt32();
		i++;
	}
	t1 = clk->GetTimeDiff();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Gen Random N = ");
	sb.AppendUOSInt(recordCnt);
	sb.Append((const UTF8Char*)", seed = ");
	sb.AppendU32(seed);
	sb.Append((const UTF8Char*)", time = ");
	Text::SBAppendF64(&sb, t1);
	sb.Append((const UTF8Char*)"s");
	console.WriteLine(sb.ToString());

/*	clk->Start();
	MemCopyANC(array2, array1, recordCnt * sizeof(Int32));
	t1 = clk->GetTimeDiff();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Memory copy time = ");
	sb.Append(t1);
	sb.Append((const UTF8Char*)"s");
	console.WriteLine(sb.ToString());

	NumberOfRec = 128;
	while (NumberOfRec <= recordCnt)
	{
		MemCopyANC(array2, array1, NumberOfRec * sizeof(Int32));
		clk->Start();
		ArtificialQuickSort_SortUInt32(array2, 0, NumberOfRec - 1);
		t1 = clk->GetTimeDiff();
		sb.ClearStr();
		sb.Append((const UTF8Char*)"AQuickSort 1 Thread N = ");
		sb.Append(NumberOfRec);
		sb.Append((const UTF8Char*)", Sort time = ");
		sb.Append(t1);
		sb.Append((const UTF8Char*)"s");
		console.Write(sb.ToString());
		valid = true;
		i = 0;
		while (i < NumberOfRec - 1)
		{
			if (array2[i] > array2[i + 1])
			{
				valid = false;
				break;
			}
			i++;
		}
		if (valid)
		{
			console.WriteLine(L", Result sorted");
		}
		else
		{
			console.WriteLine(L", Result wrong");
		}
		if (t1 >= 1.0)
		{
			break;
		}
		NumberOfRec = NumberOfRec << 1;
	}*/

	NumberOfRec = 128;
	while (NumberOfRec <= recordCnt)
	{
		MemCopyANC(array2, array1, NumberOfRec * sizeof(Int32));
		clk->Start();
		sort->SortUInt32(array2, 0, (OSInt)NumberOfRec - 1);
		t1 = clk->GetTimeDiff();
		sb.ClearStr();
		sb.Append((const UTF8Char*)"AQuickSort ");
		sb.AppendUOSInt(Sync::Thread::GetThreadCnt());
		sb.Append((const UTF8Char*)" Thread N = ");
		sb.AppendUOSInt(NumberOfRec);
		sb.Append((const UTF8Char*)", Sort time = ");
		Text::SBAppendF64(&sb, t1);
		sb.Append((const UTF8Char*)"s");
		console.Write(sb.ToString());
		valid = true;
		i = 0;
		while (i < NumberOfRec - 1)
		{
			if (array2[i] > array2[i + 1])
			{
				valid = false;
				break;
			}
			i++;
		}
		if (valid)
		{
			console.WriteLine((const UTF8Char*)", Result sorted");
		}
		else
		{
			console.WriteLine((const UTF8Char*)", Result wrong");
		}
		if (t1 >= 1.0)
		{
			break;
		}
		NumberOfRec = NumberOfRec << 1;
	}

/*	NumberOfRec = 128;
	while (NumberOfRec <= recordCnt)
	{
		MemCopyANC(array2, array1, NumberOfRec * sizeof(Int32));
		clk->Start();
		BubbleSort_SortUInt32(array2, 0, NumberOfRec - 1);
		t1 = clk->GetTimeDiff();
		sb.ClearStr();
		sb.Append((const UTF8Char*)"BubbleSortv1 1 Thread N = ");
		sb.Append(NumberOfRec);
		sb.Append((const UTF8Char*)", Sort time = ");
		sb.Append(t1);
		sb.Append((const UTF8Char*)"s");
		console.Write(sb.ToString());
		valid = true;
		i = 0;
		while (i < NumberOfRec - 1)
		{
			if (array2[i] > array2[i + 1])
			{
				valid = false;
				break;
			}
			i++;
		}
		if (valid)
		{
			console.WriteLine(L", Result sorted");
		}
		else
		{
			console.WriteLine(L", Result wrong");
		}
		if (t1 >= 1.0)
		{
			break;
		}
		NumberOfRec = NumberOfRec << 1;
	}

	NumberOfRec = 128;
	while (NumberOfRec <= recordCnt)
	{
		MemCopyANC(array2, array1, NumberOfRec * sizeof(Int32));
		clk->Start();
		InsertionSort_SortUInt32(array2, 0, NumberOfRec - 1);
		t1 = clk->GetTimeDiff();
		sb.ClearStr();
		sb.Append((const UTF8Char*)"InsertionSort 1 Thread N = ");
		sb.Append(NumberOfRec);
		sb.Append((const UTF8Char*)", Sort time = ");
		sb.Append(t1);
		sb.Append((const UTF8Char*)"s");
		console.Write(sb.ToString());
		valid = true;
		i = 0;
		while (i < NumberOfRec - 1)
		{
			if (array2[i] > array2[i + 1])
			{
				valid = false;
				break;
			}
			i++;
		}
		if (valid)
		{
			console.WriteLine(L", Result sorted");
		}
		else
		{
			console.WriteLine(L", Result wrong");
		}
		if (t1 >= 1.0)
		{
			break;
		}
		NumberOfRec = NumberOfRec << 1;
	}*/

	NumberOfRec = 128;
	while (NumberOfRec <= recordCnt)
	{
		MemCopyANC(array2, array1, NumberOfRec * sizeof(Int32));
		clk->Start();
		BitonicSort_SortUInt32(array2, 0, (OSInt)NumberOfRec - 1);
		t1 = clk->GetTimeDiff();
		sb.ClearStr();
		sb.Append((const UTF8Char*)"BitonicSort 1 Thread N = ");
		sb.AppendUOSInt(NumberOfRec);
		sb.Append((const UTF8Char*)", Sort time = ");
		Text::SBAppendF64(&sb, t1);
		sb.Append((const UTF8Char*)"s");
		console.Write(sb.ToString());
		valid = true;
		i = 0;
		while (i < NumberOfRec - 1)
		{
			if (array2[i] > array2[i + 1])
			{
				valid = false;
				break;
			}
			i++;
		}
		if (valid)
		{
			console.WriteLine((const UTF8Char*)", Result sorted");
		}
		else
		{
			console.WriteLine((const UTF8Char*)", Result wrong");
		}
		if (t1 >= 1.0)
		{
			break;
		}
		NumberOfRec = NumberOfRec << 1;
	}

	NumberOfRec = 128;
	while (NumberOfRec <= recordCnt)
	{
		MemCopyANC(array2, array1, NumberOfRec * sizeof(Int32));
		clk->Start();
		sort3->SortUInt32(array2, 0, (OSInt)NumberOfRec - 1);
		t1 = clk->GetTimeDiff();
		sb.ClearStr();
		sb.Append((const UTF8Char*)"BitonicSort ");
		sb.AppendUOSInt(Sync::Thread::GetThreadCnt());
		sb.Append((const UTF8Char*)" Thread N = ");
		sb.AppendUOSInt(NumberOfRec);
		sb.Append((const UTF8Char*)", Sort time = ");
		Text::SBAppendF64(&sb, t1);
		sb.Append((const UTF8Char*)"s");
		console.Write(sb.ToString());
		valid = true;
		i = 0;
		while (i < NumberOfRec - 1)
		{
			if (array2[i] > array2[i + 1])
			{
				valid = false;
				break;
			}
			i++;
		}
		if (valid)
		{
			console.WriteLine((const UTF8Char*)", Result sorted");
		}
		else
		{
			console.WriteLine((const UTF8Char*)", Result wrong");
		}
		if (t1 >= 1.0)
		{
			break;
		}
		NumberOfRec = NumberOfRec << 1;
	}

	NumberOfRec = 128;
	while (NumberOfRec <= recordCnt)
	{
		MemCopyANC(array2, array1, NumberOfRec * sizeof(Int32));
		clk->Start();
		ByteCountingSort_SortUInt32(array2, 0, (OSInt)NumberOfRec - 1);
		t1 = clk->GetTimeDiff();
		sb.ClearStr();
		sb.Append((const UTF8Char*)"BCountSort 1 Thread N = ");
		sb.AppendUOSInt(NumberOfRec);
		sb.Append((const UTF8Char*)", Sort time = ");
		Text::SBAppendF64(&sb, t1);
		sb.Append((const UTF8Char*)"s");
		console.Write(sb.ToString());
		valid = true;
		i = 0;
		while (i < NumberOfRec - 1)
		{
			if (array2[i] > array2[i + 1])
			{
				valid = false;
				break;
			}
			i++;
		}
		if (valid)
		{
			console.WriteLine((const UTF8Char*)", Result sorted");
		}
		else
		{
			console.WriteLine((const UTF8Char*)", Result wrong");
		}
		if (t1 >= 1.0)
		{
			break;
		}
		NumberOfRec = NumberOfRec << 1;
	}


/*	MemCopyANC(array2, array1, NumberOfRec * sizeof(Int32));
	clk->Start();
	sort2->SortInt32(array2, 0, NumberOfRec - 1);
	t1 = clk->GetTimeDiff();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Pigeonhole Sort Single Thread Sort time = ");
	sb.Append(t1);
	sb.Append((const UTF8Char*)"s");
	console.WriteLine(sb.ToString());

	valid = true;
	i = 0;
	while (i < NumberOfRec - 1)
	{
		if (array2[i] > array2[i + 1])
		{
			valid = false;
			break;
		}
		i++;
	}
	if (valid)
	{
		console.WriteLine(L"Result is sorted");
	}
	else
	{
		console.WriteLine(L"Result is not sorted");
	}*/

/*	Bool succ = false;
	Char cbuff[128];
	Char *cptr;
	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, L"SortResult.txt");
	IO::FileStream *fs;
	IO::MemoryStream *mstm;
	NEW_CLASS(mstm, IO::MemoryStream(L"mstm"));
	clk->Start();
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
	if (fs->IsError())
	{
		console.WriteLine(L"Error in writing SortResult.txt");
	}
	else
	{
		i = 0;
		while (i < NumberOfRec)
		{
			cptr = Text::StrInt32(cbuff, array2[i]);
			cptr[0] = 13;
			cptr[1] = 10;
			mstm->Write((UInt8*)cbuff, cptr - cbuff + 2);
			i++;

			if ((i & 65535) == 0)
			{
				OSInt j;
				UInt8 *buff = mstm->GetBuff(&j);
				fs->Write(buff, j);
				mstm->Clear();
			}
		}
		succ = true;
	}
	DEL_CLASS(fs);
	t1 = clk->GetTimeDiff();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"SortResult.txt writen successfully, time = ");
	sb.Append(t1);
	sb.Append((const UTF8Char*)"s");
	console.WriteLine(sb.ToString());
	DEL_CLASS(mstm);*/
	
	DEL_CLASS(sort3);
//	DEL_CLASS(sort2);
	DEL_CLASS(sort);
	DEL_CLASS(clk);
	DEL_CLASS(rand);
	MemFreeA(array1);
	MemFreeA(array2);
	return 0;
}

