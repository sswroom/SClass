#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Sort/BubbleSort.h"


void Data::Sort::BubbleSort::SortCmpO(UnsafeArray<NN<Data::Comparable>> arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt j = lastIndex - firstIndex;
	OSInt i;
	UnsafeArray<NN<Data::Comparable>> b;
	Bool finished = false;
	NN<Data::Comparable> v1;
	NN<Data::Comparable> v2;
	arr += firstIndex;
	while (j-- > 0)
	{
		finished = true;
		b = arr;
		i = j + 1;
		v1 = *b++;
		while (i--)
		{
			v2 = *b++;
			if (v1->CompareTo(v2) > 0)
			{
				b[-2] = v2;
				b[-1] = v1;
				finished = false;
			}
			else
			{
				v1 = v2;
			}
		}
		if (finished)
			break;
	}
}

void Data::Sort::BubbleSort::SortCmpOInv(UnsafeArray<NN<Data::Comparable>> arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt j = lastIndex - firstIndex;
	OSInt i;
	UnsafeArray<NN<Data::Comparable>> b;
	Bool finished = false;
	NN<Data::Comparable> v1;
	NN<Data::Comparable> v2;
	arr += firstIndex;
	while (j-- > 0)
	{
		finished = true;
		b = arr;
		i = j + 1;
		v1 = *b++;
		while (i--)
		{
			v2 = *b++;
			if (v1->CompareTo(v2) < 0)
			{
				b[-2] = v2;
				b[-1] = v1;
				finished = false;
			}
			else
			{
				v1 = v2;
			}
		}
		if (finished)
			break;
	}
}
