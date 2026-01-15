#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Comparable.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Data/Sort/BubbleSort_C.h"
#include "Data/Sort/InsertionSort_C.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

/*
Bitonic Sort
Radix Sort
*/

#if defined(_MSC_VER) && !defined(HAS_ASM32)
extern "C" Int32 UseAVX;
extern "C" Int32 CPUBrand;
#endif

void Data::Sort::ArtificialQuickSort::DoSortInt32(NN<ThreadStat> stat, UnsafeArray<Int32> arr, IntOS firstIndex, IntOS lastIndex)
{
	IntOS i;
	IntOS j;
	IntOS left;
	IntOS right;
	Int32 meja;
	IntOS left1;
	IntOS right1;
	Int32 temp1;
	Int32 temp2;
	Int32 temp;

	while (true)
	{
		while (firstIndex <= lastIndex)
		{
			left = firstIndex;
			right = lastIndex;
			i = right - left;
			if (i < 0)
			{
				break;
			}
			else if (i <= 128)
			{
				InsertionSort_SortInt32(arr.Ptr(), left, right);
				break;
			}
			else
			{
				temp1 = arr[left];
				j = left + 1;
				while (j <= right)
				{
					temp2 = arr[j];
					if (temp2 < temp1)
					{
						meja = (temp1 >> 1) + (temp2 >> 1) + ((temp1 & 1) | (temp2 & 1));
						if ( arr[ (left + right) >> 1 ] > meja )
						{
							meja = arr[ (left + right) >> 1 ];
						}
						left1 = left;
						right1 = right;
						while (true)
						{
							while (arr[right1] >= meja)
							{
								if (--right1 < left1)
									break;
							}
							while ( arr[left1] < meja )
							{
								if (++left1 > right1)
									break;
							}
							if (left1 > right1)
								break;

							temp = arr[right1];
							arr[right1--] = arr[left1];
							arr[left1++] = temp;
						}
						if (left1 == left)
						{
							arr[(left + right) >> 1] = arr[left];
							arr[left] = meja;
							firstIndex = left + 1;
							lastIndex = right;
						}
						else
						{
							lastIndex = --left1;
							++right1;
							if (right1 < right)
							{
								Sync::MutexUsage mutUsage(this->mut);
								this->tasks[this->taskCnt * 2] = right1;
								this->tasks[this->taskCnt * 2 + 1] = right;
								this->taskCnt++;
								mutUsage.EndUse();
								if (stat->threadId + 1 < this->threadCnt)
								{
									if (this->threads[stat->threadId + 1].state == 1)
									{
										this->threads[stat->threadId + 1].evt->Set();
									}
								}
							}
						}

						break;
					}
					temp1 = temp2;
					j++;
				}
				if (j > right)
				{
					break;
				}
			}
		}

		Bool found = false;
		{
			Sync::MutexUsage mutUsage(this->mut);
			if (this->taskCnt > 0)
			{
				found = true;
				firstIndex = this->tasks[this->taskCnt * 2 - 2];
				lastIndex = this->tasks[this->taskCnt * 2 - 1];
				this->taskCnt--;
			}
			mutUsage.EndUse();
		}
		if (!found)
			return;
	}
}

void Data::Sort::ArtificialQuickSort::DoSortUInt32(NN<ThreadStat> stat, UnsafeArray<UInt32> arr, IntOS firstIndex, IntOS lastIndex)
{
	IntOS i;
	IntOS j;
	IntOS left;
	IntOS right;
	UInt32 meja;
	IntOS left1;
	IntOS right1;
	UInt32 temp1;
	UInt32 temp2;
	UInt32 temp;

	while (true)
	{
		while (firstIndex <= lastIndex)
		{
			left = firstIndex;
			right = lastIndex;
			i = right - left;
			if (i < 0)
			{
				break;
			}
			else if (i <= 128)
			{
				InsertionSort_SortUInt32(arr.Ptr(), left, right);
				break;
			}
			else
			{
				temp1 = arr[left];
				j = left + 1;
				while (j <= right)
				{
					temp2 = arr[j];
					if (temp2 < temp1)
					{
						meja = (temp1 >> 1) + (temp2 >> 1) + ((temp1 & 1) | (temp2 & 1));
						if ( arr[ (left + right) >> 1 ] > meja )
						{
							meja = arr[ (left + right) >> 1 ];
						}
						left1 = left;
						right1 = right;
						while (true)
						{
							while (arr[right1] >= meja)
							{
								if (--right1 < left1)
									break;
							}
							while ( arr[left1] < meja )
							{
								if (++left1 > right1)
									break;
							}
							if (left1 > right1)
								break;

							temp = arr[right1];
							arr[right1--] = arr[left1];
							arr[left1++] = temp;
						}
						if (left1 == left)
						{
							arr[(left + right) >> 1] = arr[left];
							arr[left] = meja;
							firstIndex = left + 1;
							lastIndex = right;
						}
						else
						{
							lastIndex = --left1;
							++right1;
							if (right1 < right)
							{
								Sync::MutexUsage mutUsage(this->mut);
								this->tasks[this->taskCnt * 2] = right1;
								this->tasks[this->taskCnt * 2 + 1] = right;
								this->taskCnt++;
								mutUsage.EndUse();
								if (stat->threadId + 1 < this->threadCnt)
								{
									if (this->threads[stat->threadId + 1].state == 1)
									{
										this->threads[stat->threadId + 1].evt->Set();
									}
								}
							}
						}

						break;
					}
					temp1 = temp2;
					j++;
				}
				if (j > right)
				{
					break;
				}
			}
		}

		Bool found = false;
		Sync::MutexUsage mutUsage(this->mut);
		if (this->taskCnt > 0)
		{
			found = true;
			firstIndex = this->tasks[this->taskCnt * 2 - 2];
			lastIndex = this->tasks[this->taskCnt * 2 - 1];
			this->taskCnt--;
		}
		mutUsage.EndUse();
		if (!found)
			return;
	}
}

void Data::Sort::ArtificialQuickSort::DoSortStr(NN<ThreadStat> stat, UnsafeArray<UnsafeArray<UTF8Char>> arr, IntOS firstIndex, IntOS lastIndex)
{
	IntOS i;
	IntOS left;
	IntOS right;
	UnsafeArray<UTF8Char> meja;
	IntOS left1;
	IntOS right1;
	UnsafeArray<UTF8Char> temp;

	while (true)
	{
		while (firstIndex <= lastIndex)
		{
			left = firstIndex;
			right = lastIndex;
			i = right - left;
			if (i < 0)
			{
				break;
			}
			else if (i <= 128)
			{
				InsertionSort_SortBStr((UTF8Char**)arr.Ptr(), left, right);
				break;
			}
			else
			{
				meja = arr[ (left + right) >> 1 ];
				left1 = left;
				right1 = right;
				while (true)
				{
					while (Text::StrCompare(arr[right1], meja) >= 0)
					{
						if (--right1 < left1)
							break;
					}
					while (Text::StrCompare(arr[left1], meja) < 0)
					{
						if (++left1 > right1)
							break;
					}
					if (left1 > right1)
						break;

					temp = arr[right1];
					arr[right1--] = arr[left1];
					arr[left1++] = temp;
				}
				if (left1 == left)
				{
					arr[(left + right) >> 1] = arr[left];
					arr[left] = meja;
					firstIndex = left + 1;
					lastIndex = right;
				}
				else
				{
					lastIndex = --left1;
					++right1;
					if (right1 < right)
					{
						Sync::MutexUsage mutUsage(this->mut);
						this->tasks[this->taskCnt * 2] = right1;
						this->tasks[this->taskCnt * 2 + 1] = right;
						this->taskCnt++;
						mutUsage.EndUse();
						if (stat->threadId + 1 < this->threadCnt)
						{
							if (this->threads[stat->threadId + 1].state == 1)
							{
								this->threads[stat->threadId + 1].evt->Set();
							}
						}
					}
				}
			}
		}

		Bool found = false;
		Sync::MutexUsage mutUsage(this->mut);
		if (this->taskCnt > 0)
		{
			found = true;
			firstIndex = this->tasks[this->taskCnt * 2 - 2];
			lastIndex = this->tasks[this->taskCnt * 2 - 1];
			this->taskCnt--;
		}
		mutUsage.EndUse();
		if (!found)
			return;
	}
}

UInt32 __stdcall Data::Sort::ArtificialQuickSort::ProcessThread(AnyType userObj)
{
	NN<ThreadStat> stat = userObj.GetNN<ThreadStat>();
	Bool found;
	IntOS firstIndex;
	IntOS lastIndex;
	UnsafeArray<UInt8> arr;
	{
		Sync::Event evt;
		stat->evt = evt;
		stat->state = 1;
		while (!stat->toStop)
		{
			if (stat->me->taskCnt > 0)
			{
				if (stat->me->arrType == AT_INT32 && stat->me->arr.SetTo(arr))
				{
					stat->state = 2;
					while (stat->me->taskCnt > 0)
					{
						Sync::MutexUsage mutUsage(stat->me->mut);
						if (stat->me->taskCnt > 0)
						{
							found = true;
							firstIndex = stat->me->tasks[stat->me->taskCnt * 2 - 2];
							lastIndex = stat->me->tasks[stat->me->taskCnt * 2 - 1];
							stat->me->taskCnt--;
						}
						else
						{
							found = false;
						}
						mutUsage.EndUse();

						if (!found)
						{
							break;
						}
						stat->me->DoSortInt32(stat, UnsafeArray<Int32>::ConvertFrom(arr), firstIndex, lastIndex);
					}
					stat->state = 1;
					stat->me->mainEvt.Set();
				}
				else if (stat->me->arrType == AT_UINT32 && stat->me->arr.SetTo(arr))
				{
					stat->state = 2;
					while (stat->me->taskCnt > 0)
					{
						Sync::MutexUsage mutUsage(stat->me->mut);
						if (stat->me->taskCnt > 0)
						{
							found = true;
							firstIndex = stat->me->tasks[stat->me->taskCnt * 2 - 2];
							lastIndex = stat->me->tasks[stat->me->taskCnt * 2 - 1];
							stat->me->taskCnt--;
						}
						else
						{
							found = false;
						}
						mutUsage.EndUse();

						if (!found)
						{
							break;
						}
						stat->me->DoSortUInt32(stat, UnsafeArray<UInt32>::ConvertFrom(arr), firstIndex, lastIndex);
					}
					stat->state = 1;
					stat->me->mainEvt.Set();
				}
				else if (stat->me->arrType == AT_STRUTF8 && stat->me->arr.SetTo(arr))
				{
					stat->state = 2;
					while (stat->me->taskCnt > 0)
					{
						Sync::MutexUsage mutUsage(stat->me->mut);
						if (stat->me->taskCnt > 0)
						{
							found = true;
							firstIndex = stat->me->tasks[stat->me->taskCnt * 2 - 2];
							lastIndex = stat->me->tasks[stat->me->taskCnt * 2 - 1];
							stat->me->taskCnt--;
						}
						else
						{
							found = false;
						}
						mutUsage.EndUse();

						if (!found)
						{
							break;
						}
						stat->me->DoSortStr(stat, UnsafeArray<UnsafeArray<UTF8Char>>::ConvertFrom(arr), firstIndex, lastIndex);
					}
					stat->state = 1;
					stat->me->mainEvt.Set();
				}
			}
			stat->evt->Wait(1000);
		}

	}
	stat->state = 0;
	stat->me->mainEvt.Set();
	return 0;
}

Data::Sort::ArtificialQuickSort::ArtificialQuickSort()
{
	this->threadCnt = Sync::ThreadUtil::GetThreadCnt();
	this->arr = nullptr;
	this->tasks = MemAllocArr(IntOS, 65536);
	this->taskCnt = 0;
	this->threads = MemAllocArr(ThreadStat, this->threadCnt);
	UIntOS i;
	i = this->threadCnt;
	while (i-- > 0)
	{
		this->threads[i].me = *this;
		this->threads[i].state = 0;
		this->threads[i].toStop = false;
		this->threads[i].threadId = i;

		Sync::ThreadUtil::Create(ProcessThread, &this->threads[i]);
	}

	while (true)
	{
		Bool found;

		found = false;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (this->threads[i].state == 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
			break;
		this->mainEvt.Wait(10);
	}
}

Data::Sort::ArtificialQuickSort::~ArtificialQuickSort()
{
	UIntOS i;
	i = this->threadCnt;
	while (i-- > 0)
	{
		this->threads[i].toStop = true;
		this->threads[i].evt->Set();
	}
	
	while (true)
	{
		Bool found;
		found = false;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (this->threads[i].state != 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
			break;
		this->mainEvt.Wait(10);
	}	
	MemFreeArr(this->tasks);
	MemFreeArr(this->threads);
}

void Data::Sort::ArtificialQuickSort::SortInt32(UnsafeArray<Int32> arr, IntOS firstIndex, IntOS lastIndex)
{
	this->arr = UnsafeArray<UInt8>::ConvertFrom(arr);
	this->arrType = AT_INT32;
	ArtificialQuickSort_PreSortInt32(arr.Ptr(), firstIndex, lastIndex);
	Sync::MutexUsage mutUsage(this->mut);
	this->tasks[0] = firstIndex;
	this->tasks[1] = lastIndex;
	this->taskCnt = 1;
	mutUsage.EndUse();
	this->threads[0].evt->Set();

	while (true)
	{
		Bool found;
		this->mainEvt.Wait(1000);
		found = false;
		if (this->taskCnt > 0)
		{
			found = true;
		}
		else
		{
			UIntOS i = this->threadCnt;
			while (i-- > 0)
			{
				if (this->threads[i].state != 1)
				{
					found = true;
					break;
				}
			}
			if (this->taskCnt > 0)
			{
				found = true;
			}
		}

		if (!found)
		{
			break;
		}
	}
	this->arr = nullptr;
}

void Data::Sort::ArtificialQuickSort::SortUInt32(UnsafeArray<UInt32> arr, IntOS firstIndex, IntOS lastIndex)
{
	this->arr = UnsafeArray<UInt8>::ConvertFrom(arr);
	this->arrType = AT_UINT32;
	ArtificialQuickSort_PreSortUInt32(arr.Ptr(), firstIndex, lastIndex);
	Sync::MutexUsage mutUsage(this->mut);
	this->tasks[0] = firstIndex;
	this->tasks[1] = lastIndex;
	this->taskCnt = 1;
	mutUsage.EndUse();
	this->threads[0].evt->Set();

	while (true)
	{
		Bool found;
		this->mainEvt.Wait(1000);
		found = false;
		if (this->taskCnt > 0)
		{
			found = true;
		}
		else
		{
			UIntOS i = this->threadCnt;
			while (i-- > 0)
			{
				if (this->threads[i].state != 1)
				{
					found = true;
					break;
				}
			}
			if (this->taskCnt > 0)
			{
				found = true;
			}
		}

		if (!found)
		{
			break;
		}
	}
	this->arr = nullptr;
}

void Data::Sort::ArtificialQuickSort::SortStr(UnsafeArray<UnsafeArray<UTF8Char>> arr, IntOS firstIndex, IntOS lastIndex)
{
	this->arr = UnsafeArray<UInt8>::ConvertFrom(arr);
	this->arrType = AT_STRUTF8;
	ArtificialQuickSort_PreSortStr((UTF8Char**)arr.Ptr(), firstIndex, lastIndex);
	Sync::MutexUsage mutUsage(this->mut);
	this->tasks[0] = firstIndex;
	this->tasks[1] = lastIndex;
	this->taskCnt = 1;
	mutUsage.EndUse();
	this->threads[0].evt->Set();

	while (true)
	{
		Bool found;
		this->mainEvt.Wait(1000);
		found = false;
		if (this->taskCnt > 0)
		{
			found = true;
		}
		else
		{
			UIntOS i = this->threadCnt;
			while (i-- > 0)
			{
				if (this->threads[i].state != 1)
				{
					found = true;
					break;
				}
			}
			if (this->taskCnt > 0)
			{
				found = true;
			}
		}

		if (!found)
		{
			break;
		}
	}
	this->arr = nullptr;
}

/*
void ArtificialQuickSort(int array[], int nor) {

    int levi[256];
    int desni[256];

    int index = 0; int i = 0; int j = 0;
    int left = 0; int right = 0; int meja = 0;
    int left1 = 0; int right1 = 0;
    int temp1 = 0; int temp2 = 0; int temp = 0;

    left = 0; right = nor-1;

    while (left < right){
        if (array[left] > array[right]) {
            temp = array[left];
            array[left] = array[right];
            array[right] = temp;
        }
        left++; right--;
    }

    index = 0; levi[index] = 0; desni[index] = nor-1;

    while ( index >= 0 ) {
        left = levi[index]; right = desni[index];
        temp = right - left;
        if ( temp > 15 ) {

// Main section of ArtificialQuickSort -------------------------------------------
            temp1 = array[left];
            for ( j = left+1; j <= right; j++) {
                temp2 = array[j];
                if ( temp2 < temp1) {
                    meja = ( temp1 + temp2 + 1 ) >> 1;
                    if ( array[ (left + right) >> 1 ] > meja ) {
                        meja = array[ (left + right) >> 1 ];
                    }
                    left1 = left;
                    right1 = right;
                    for (;;) {
                        while ( array[right1] >= meja ) right1--;
                        while ( array[left1] < meja ) left1++;
                        if (left1>right1) break;

                        temp = array[right1];
                        array[right1--] = array[left1];
                        array[left1++] = temp;
                    }
                    desni[index] = --left1;
                    levi[++index] = ++right1;
                    desni[index] = right;
                    goto labelcritticall5;
                }
                temp1=temp2;
            }
// End of the main section of ArtificialQuickSort --------------------------------

        } else {

            if ( temp != 0 ) {

// InsertionSort -----------------------------------------------------------
                temp1 = array[left];
                for ( i = left+1; i <= right; ++i ) {
                    temp2 = array[i];
                    if ( temp1 > temp2 ) {
                        array[i] = temp1;
                        for ( j = i-1; j > left; ) {
                            temp = array[j-1];
                            if ( temp > temp2 ) array[j--] = temp; else break;
                        }
                        array[j] = temp2;
                    } else {
                        temp1 = temp2;
                    }
                }
// End of InsertionSort -----------------------------------------------------

            }
        }
        index--;
        labelcritticall5:;

    }

}
*/


void Data::Sort::ArtificialQuickSort::PreSortCmpO(UnsafeArray<NN<Data::Comparable>> arr, IntOS left, IntOS right)
{
	NN<Data::Comparable> temp;

	while (left < right)
	{
		if (arr[left]->CompareTo(arr[right]) > 0)
		{
			temp = arr[left];
			arr[left] = arr[right];
			arr[right] = temp;
		}
		left++;
		right--;
	}
}

void Data::Sort::ArtificialQuickSort::SortCmpO(UnsafeArray<NN<Data::Comparable>> arr, IntOS firstIndex, IntOS lastIndex)
{
#if _OSINT_SIZE == 16
	IntOS levi[256];
	IntOS desni[256];
#else
	IntOS *levi = MemAlloc(IntOS, 65536);
	IntOS *desni = &levi[32768];
#endif
	IntOS index;
	IntOS i;
	IntOS left;
	IntOS right;
	NN<Data::Comparable> meja;
	IntOS left1;
	IntOS right1;
	NN<Data::Comparable> temp;

	PreSortCmpO(arr, firstIndex, lastIndex);

	index = 0;
	levi[index] = firstIndex;
	desni[index] = lastIndex;

	while ( index >= 0 )
	{
		left = levi[index];
		right = desni[index];
		i = right - left;
		if (i <= 0)
		{
			index--;
		}
		else if (i <= 64)
		{
			Data::Sort::InsertionSort::SortBCmpO(arr, left, right);
			index--;
		}
		else
		{
			meja = arr[ (left + right) >> 1 ];
			left1 = left;
			right1 = right;
			while (true)
			{
				while ( arr[right1]->CompareTo(meja) >= 0 )
				{
					if (--right1 < left1)
						break;
				}
				while ( arr[left1]->CompareTo(meja) < 0 )
				{
					if (++left1 > right1)
						break;
				}
				if (left1 > right1)
					break;

				temp = arr[right1];
				arr[right1--] = arr[left1];
				arr[left1++] = temp;
			}
			if (left1 == left)
			{
				arr[(left + right) >> 1] = arr[left];
				arr[left] = meja;
				levi[index] = left + 1;
				desni[index] = right;
			}
			else
			{
				desni[index] = --left1;
				right1++;
				index++;
				levi[index] = right1;
				desni[index] = right;
			}
		}
	}
#if _OSINT_SIZE != 16
	MemFree(levi);
#endif
}
