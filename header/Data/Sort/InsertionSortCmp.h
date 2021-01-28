#ifndef _SM_DATA_SORT_INSERTIONSORTCMP
#define _SM_DATA_SORT_INSERTIONSORTCMP
#include "Data/ArrayList.h"
#include "Data/DataComparer.h"

template <class T> void InsertionSort_SortB(Data::ArrayList<T> *arr, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	T temp;
	T temp1;
	T temp2;
	temp1 = arr->GetItem(left);
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr->GetItem(i);
		if ( Data::DataComparer::Compare(temp1, temp2) > 0)
		{
			j = left;
			k = i - 1;
			while (j <= k)
			{
				l = (j + k) >> 1;
				temp = arr->GetItem(l);
				if (Data::DataComparer::Compare(temp, temp2) > 0)
				{
					k = l - 1;
				}
				else
				{
					j = l + 1;
				}
			}
			arr->CopyItems(j + 1, j, i - j);
			arr->SetItem(j, temp2);
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}

#endif
