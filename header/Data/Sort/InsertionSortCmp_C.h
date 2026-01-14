#ifndef _SM_DATA_SORT_INSERTIONSORTCMP_C
#define _SM_DATA_SORT_INSERTIONSORTCMP_C
#include "Data/ArrayListNative.hpp"
#include "Data/DataComparer.h"

template <class T> void InsertionSort_SortB(T *arr, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	T temp;
	T temp1;
	T temp2;
	temp1 = arr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr[i];
		if ( Data::DataComparer::Compare(temp1, temp2) > 0)
		{
			j = left;
			k = i - 1;
			while (j <= k)
			{
				l = (j + k) >> 1;
				temp = arr[l];
				if (Data::DataComparer::Compare(temp, temp2) > 0)
				{
					k = l - 1;
				}
				else
				{
					j = l + 1;
				}
			}
			MemCopyO(&arr[j + 1], &arr[j], (UOSInt)(i - j) * sizeof(T));
			arr[j] = temp2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}

template <class T> void InsertionSort_SortB(Data::ArrayListNative<T> *arr, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	T temp;
	T temp1;
	T temp2;
	temp1 = arr->GetItem((UOSInt)left);
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr->GetItem((UOSInt)i);
		if ( Data::DataComparer::Compare(temp1, temp2) > 0)
		{
			j = left;
			k = i - 1;
			while (j <= k)
			{
				l = (j + k) >> 1;
				temp = arr->GetItem((UOSInt)l);
				if (Data::DataComparer::Compare(temp, temp2) > 0)
				{
					k = l - 1;
				}
				else
				{
					j = l + 1;
				}
			}
			arr->CopyItems((UOSInt)j + 1, (UOSInt)j, (UOSInt)(i - j));
			arr->SetItem((UOSInt)j, temp2);
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}

#endif
