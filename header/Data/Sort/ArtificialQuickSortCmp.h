#ifndef _SM_DATA_SORT_ARTIFICIALQUICKSORTCMP
#define _SM_DATA_SORT_ARTIFICIALQUICKSORTCMP
#include "Data/DataComparer.h"
#include "Data/ArrayList.h"
#include "Data/Sort/InsertionSortCmp.h"

template <class T> void ArtificialQuickSort_PreSort(Data::ArrayList<T> *arr, OSInt left, OSInt right);
template <class T> void ArtificialQuickSort_Sort(Data::ArrayList<T> *arr, OSInt firstIndex, OSInt lastIndex);

template <class T> void ArtificialQuickSort_PreSort(Data::ArrayList<T> *arr, OSInt left, OSInt right)
{
	T temp = 0;
	T temp2;

	while (left < right)
	{
		temp = arr->GetItem(left);
		temp2 = arr->GetItem(right);
		if (Data::DataComparer::Compare(temp, temp2) > 0)
		{
			arr->SetItem((UOSInt)left, temp2);
			arr->SetItem((UOSInt)right, temp);
		}
		left++;
		right--;
	}
}

template <class T> void ArtificialQuickSort_Sort(Data::ArrayList<T> *arr, OSInt firstIndex, OSInt lastIndex)
{
#if _OSINT_SIZE == 16
	OSInt levi[256];
	OSInt desni[256];
#else
	OSInt *levi = MemAlloc(OSInt, 65536);
	OSInt *desni = &levi[32768];
#endif
	OSInt index;
	OSInt i;
	OSInt left;
	OSInt right;
	T meja;
	OSInt left1;
	OSInt right1;
	T temp;

	ArtificialQuickSort_PreSort(arr, firstIndex, lastIndex);

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
			InsertionSort_SortB(arr, left, right);
			index--;
		}
		else
		{
			meja = arr->GetItem( (UOSInt)(left + right) >> 1 );
			left1 = left;
			right1 = right;
			while (true)
			{
				while ( Data::DataComparer::Compare(arr->GetItem((UOSInt)right1), meja) >= 0 )
				{
					if (--right1 < left1)
						break;
				}
				while ( Data::DataComparer::Compare(arr->GetItem((UOSInt)left1), meja) < 0 )
				{
					if (++left1 > right1)
						break;
				}
				if (left1 > right1)
					break;

				temp = arr->GetItem(right1);
				arr->SetItem((UOSInt)right1--, arr->GetItem((UOSInt)left1));
				arr->SetItem((UOSInt)left1++, temp);
			}
			if (left1 == left)
			{
				arr->SetItem((UOSInt)(left + right) >> 1, arr->GetItem((UOSInt)left));
				arr->SetItem((UOSInt)left, meja);
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

#endif
