#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/IComparable.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Data/Sort/BubbleSort.h"
#include "Data/Sort/InsertionSort.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

/*
Bitonic Sort
Radix Sort
*/

#if defined(_MSC_VER) && !defined(HAS_ASM32)
extern "C" Int32 UseAVX;
extern "C" Int32 CPUBrand;
#endif

void Data::Sort::ArtificialQuickSort::DoSortInt32(ThreadStat *stat, Int32 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt i;
	OSInt j;
	OSInt left;
	OSInt right;
	Int32 meja;
	OSInt left1;
	OSInt right1;
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
				InsertionSort_SortInt32(arr, left, right);
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
								this->mut->Lock();
								this->tasks[this->taskCnt * 2] = right1;
								this->tasks[this->taskCnt * 2 + 1] = right;
								this->taskCnt++;
								this->mut->Unlock();
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
		this->mut->Lock();
		if (this->taskCnt > 0)
		{
			found = true;
			firstIndex = this->tasks[this->taskCnt * 2 - 2];
			lastIndex = this->tasks[this->taskCnt * 2 - 1];
			this->taskCnt--;
		}
		this->mut->Unlock();
		if (!found)
			return;
	}
}

void Data::Sort::ArtificialQuickSort::DoSortUInt32(ThreadStat *stat, UInt32 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt i;
	OSInt j;
	OSInt left;
	OSInt right;
	UInt32 meja;
	OSInt left1;
	OSInt right1;
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
				InsertionSort_SortUInt32(arr, left, right);
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
								this->mut->Lock();
								this->tasks[this->taskCnt * 2] = right1;
								this->tasks[this->taskCnt * 2 + 1] = right;
								this->taskCnt++;
								this->mut->Unlock();
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
		this->mut->Lock();
		if (this->taskCnt > 0)
		{
			found = true;
			firstIndex = this->tasks[this->taskCnt * 2 - 2];
			lastIndex = this->tasks[this->taskCnt * 2 - 1];
			this->taskCnt--;
		}
		this->mut->Unlock();
		if (!found)
			return;
	}
}

void Data::Sort::ArtificialQuickSort::DoSortStr(ThreadStat *stat, UTF8Char **arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt i;
	OSInt left;
	OSInt right;
	UTF8Char *meja;
	OSInt left1;
	OSInt right1;
	UTF8Char *temp;

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
				InsertionSort_SortBStr(arr, left, right);
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
						this->mut->Lock();
						this->tasks[this->taskCnt * 2] = right1;
						this->tasks[this->taskCnt * 2 + 1] = right;
						this->taskCnt++;
						this->mut->Unlock();
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
		this->mut->Lock();
		if (this->taskCnt > 0)
		{
			found = true;
			firstIndex = this->tasks[this->taskCnt * 2 - 2];
			lastIndex = this->tasks[this->taskCnt * 2 - 1];
			this->taskCnt--;
		}
		this->mut->Unlock();
		if (!found)
			return;
	}
}

UInt32 __stdcall Data::Sort::ArtificialQuickSort::ProcessThread(void *userObj)
{
	ThreadStat *stat = (ThreadStat*)userObj;
	Bool found;
	OSInt firstIndex;
	OSInt lastIndex;
	stat->state = 1;
	while (!stat->toStop)
	{
		if (stat->me->taskCnt > 0)
		{
			if (stat->me->arrType == AT_INT32)
			{
				stat->state = 2;
				while (stat->me->taskCnt > 0)
				{
					stat->me->mut->Lock();
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
					stat->me->mut->Unlock();

					if (!found)
					{
						break;
					}
					stat->me->DoSortInt32(stat, (Int32*)stat->me->arr, firstIndex, lastIndex);
				}
				stat->state = 1;
				stat->me->mainEvt->Set();
			}
			else if (stat->me->arrType == AT_UINT32)
			{
				stat->state = 2;
				while (stat->me->taskCnt > 0)
				{
					stat->me->mut->Lock();
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
					stat->me->mut->Unlock();

					if (!found)
					{
						break;
					}
					stat->me->DoSortUInt32(stat, (UInt32*)stat->me->arr, firstIndex, lastIndex);
				}
				stat->state = 1;
				stat->me->mainEvt->Set();
			}
			else if (stat->me->arrType == AT_STRUTF8)
			{
				stat->state = 2;
				while (stat->me->taskCnt > 0)
				{
					stat->me->mut->Lock();
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
					stat->me->mut->Unlock();

					if (!found)
					{
						break;
					}
					stat->me->DoSortStr(stat, (UTF8Char**)stat->me->arr, firstIndex, lastIndex);
				}
				stat->state = 1;
				stat->me->mainEvt->Set();
			}
		}
		stat->evt->Wait(1000);
	}
	stat->state = 0;
	stat->me->mainEvt->Set();
	return 0;
}

Data::Sort::ArtificialQuickSort::ArtificialQuickSort()
{
	this->threadCnt = Sync::Thread::GetThreadCnt();
	NEW_CLASS(this->mainEvt, Sync::Event(true, (const UTF8Char*)"Data.Sort.ArtificialQuickSort.mainEvt"));
	NEW_CLASS(this->mut, Sync::Mutex());
	this->arr = 0;
	this->tasks = MemAlloc(OSInt, 65536);
	this->taskCnt = 0;
	this->threads = MemAlloc(ThreadStat, this->threadCnt);
	OSInt i;
	i = this->threadCnt;
	while (i-- > 0)
	{
		this->threads[i].me = this;
		this->threads[i].state = 0;
		this->threads[i].toStop = false;
		this->threads[i].threadId = i;
		NEW_CLASS(this->threads[i].evt, Sync::Event(true, (const UTF8Char*)"Data.Sort.ArtificialQuickSort.threads.evt"));

		Sync::Thread::Create(ProcessThread, &this->threads[i]);
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
		this->mainEvt->Wait(10);
	}
}

Data::Sort::ArtificialQuickSort::~ArtificialQuickSort()
{
	OSInt i;
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
		this->mainEvt->Wait(10);
	}
	i = this->threadCnt;
	while (i-- > 0)
	{
		DEL_CLASS(this->threads[i].evt);
	}
	
	DEL_CLASS(this->mainEvt);
	DEL_CLASS(this->mut);
	MemFree(this->tasks);
	MemFree(this->threads);
}

void Data::Sort::ArtificialQuickSort::SortInt32(Int32 *arr, OSInt firstIndex, OSInt lastIndex)
{
	this->arr = arr;
	this->arrType = AT_INT32;
	ArtificialQuickSort_PreSortInt32(arr, firstIndex, lastIndex);
	this->mut->Lock();
	this->tasks[0] = firstIndex;
	this->tasks[1] = lastIndex;
	this->taskCnt = 1;
	this->mut->Unlock();
	this->threads[0].evt->Set();

	while (true)
	{
		Bool found;
		this->mainEvt->Wait(1000);
		found = false;
		if (this->taskCnt > 0)
		{
			found = true;
		}
		else
		{
			OSInt i = this->threadCnt;
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
	this->arr = 0;
}

void Data::Sort::ArtificialQuickSort::SortUInt32(UInt32 *arr, OSInt firstIndex, OSInt lastIndex)
{
	this->arr = arr;
	this->arrType = AT_UINT32;
	ArtificialQuickSort_PreSortUInt32(arr, firstIndex, lastIndex);
	this->mut->Lock();
	this->tasks[0] = firstIndex;
	this->tasks[1] = lastIndex;
	this->taskCnt = 1;
	this->mut->Unlock();
	this->threads[0].evt->Set();

	while (true)
	{
		Bool found;
		this->mainEvt->Wait(1000);
		found = false;
		if (this->taskCnt > 0)
		{
			found = true;
		}
		else
		{
			OSInt i = this->threadCnt;
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
	this->arr = 0;
}

void Data::Sort::ArtificialQuickSort::SortStr(UTF8Char **arr, OSInt firstIndex, OSInt lastIndex)
{
	this->arr = arr;
	this->arrType = AT_STRUTF8;
	ArtificialQuickSort_PreSortStr(arr, firstIndex, lastIndex);
	this->mut->Lock();
	this->tasks[0] = firstIndex;
	this->tasks[1] = lastIndex;
	this->taskCnt = 1;
	this->mut->Unlock();
	this->threads[0].evt->Set();

	while (true)
	{
		Bool found;
		this->mainEvt->Wait(1000);
		found = false;
		if (this->taskCnt > 0)
		{
			found = true;
		}
		else
		{
			OSInt i = this->threadCnt;
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
	this->arr = 0;
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

