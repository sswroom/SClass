#include "Stdafx.h"
#include "Data/Sort/InsertionSort.h"

void Data::Sort::InsertionSort::SortBCmpO(UnsafeArray<NN<Data::IComparable>> arr, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	NN<Data::IComparable> temp;
	NN<Data::IComparable> temp1;
	NN<Data::IComparable> temp2;
	temp1 = arr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr[i];
		if ( temp1->CompareTo(temp2) > 0)
		{
			j = left;
			k = i - 1;
			while (j <= k)
			{
				l = (j + k) >> 1;
				temp = arr[l];
				if (temp->CompareTo(temp2) > 0)
				{
					k = l - 1;
				}
				else
				{
					j = l + 1;
				}
			}
			MemCopyO(&arr[j + 1], &arr[j], (UOSInt)(i - j) * sizeof(arr[0]));
			arr[j] = temp2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}
