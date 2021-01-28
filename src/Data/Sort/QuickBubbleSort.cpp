#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/IComparable.h"
#include "Data/Sort/QuickBubbleSort.h"
#include "Data/Sort/BubbleSort.h"

void Data::Sort::QuickBubbleSort::Sort(Int32 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt lo = firstIndex;
	OSInt hi = lastIndex;
	Int32 pivot;
	Int32 tmp;
	Int32 *tmparr;

	while (hi > lo)
	{
		if ((hi - lo) <= 6)
		{
			BubbleSort_SortInt32(arr, lo, hi);
			return;
		}

		pivot = arr[(lo + hi) >> 1];
		arr[(lo + hi) >> 1] = arr[hi];
		arr[hi--] = pivot;

		while( lo <= hi )
		{
			tmparr = &arr[lo];
			while (*tmparr++ <= pivot)
				if ((++lo) >= hi)
					break;
			tmparr = &arr[hi];
			while (pivot <= *tmparr--)
				if (lo >= (--hi))
					break;
			if (lo < hi)
			{
				tmp = arr[lo];
				arr[lo] = arr[hi];
				arr[hi] = tmp;
			}
		}
		tmparr = &arr[hi];
		if (hi < firstIndex || *tmparr < pivot)
		{
			arr[lastIndex] = *++tmparr;
			*tmparr = pivot;
		}
		else
		{
			arr[lastIndex] = *tmparr;
			*tmparr = pivot;
		}

		if ((lo - firstIndex) > (lastIndex - hi))
		{
			Sort(arr, hi+1, lastIndex);
			hi = lastIndex = lo - 1;
			lo = firstIndex;
		}
		else
		{
			Sort(arr, firstIndex, lo-1);
			lo = firstIndex = hi + 1;
			hi = lastIndex;
		}
	}
}

void Data::Sort::QuickBubbleSort::Sort(UInt32 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt lo = firstIndex;
	OSInt hi = lastIndex;
	UInt32 pivot;
	UInt32 tmp;
	UInt32 *tmparr;

	while (hi > lo)
	{
		if ((hi - lo) <= 6)
		{
			BubbleSort_SortUInt32(arr, lo, hi);
			return;
		}

		pivot = arr[(lo + hi) >> 1];
		arr[(lo + hi) >> 1] = arr[hi];
		arr[hi--] = pivot;

		while( lo <= hi )
		{
			tmparr = &arr[lo];
			while (*tmparr++ <= pivot)
				if ((++lo) >= hi)
					break;
			tmparr = &arr[hi];
			while (pivot <= *tmparr--)
				if (lo >= (--hi))
					break;
			if (lo < hi)
			{
				tmp = arr[lo];
				arr[lo] = arr[hi];
				arr[hi] = tmp;
			}
		}
		tmparr = &arr[hi];
		if (hi < firstIndex || *tmparr < pivot)
		{
			arr[lastIndex] = *++tmparr;
			*tmparr = pivot;
		}
		else
		{
			arr[lastIndex] = *tmparr;
			*tmparr = pivot;
		}

		if ((lo - firstIndex) > (lastIndex - hi))
		{
			Sort(arr, hi+1, lastIndex);
			hi = lastIndex = lo - 1;
			lo = firstIndex;
		}
		else
		{
			Sort(arr, firstIndex, lo-1);
			lo = firstIndex = hi + 1;
			hi = lastIndex;
		}
	}
}

void Data::Sort::QuickBubbleSort::Sort(Double *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt lo = firstIndex;
	OSInt hi = lastIndex;
	Double pivot;
	Double tmp;
	Double *tmparr;

	while (hi > lo)
	{
		if ((hi - lo) <= 6)
		{
			BubbleSort_SortDouble(arr, lo, hi);
			return;
		}

		pivot = arr[(lo + hi) >> 1];
		arr[(lo + hi) >> 1] = arr[hi];
		arr[hi--] = pivot;

		while( lo <= hi )
		{
			tmparr = &arr[lo];
			while (*tmparr++ <= pivot)
				if ((++lo) >= hi)
					break;
			tmparr = &arr[hi];
			while (pivot <= *tmparr--)
				if (lo >= (--hi))
					break;
			if (lo < hi)
			{
				tmp = arr[lo];
				arr[lo] = arr[hi];
				arr[hi] = tmp;
			}
		}
		tmparr = &arr[hi];
		if (hi < firstIndex || *tmparr < pivot)
		{
			arr[lastIndex] = *++tmparr;
			*tmparr = pivot;
		}
		else
		{
			arr[lastIndex] = *tmparr;
			*tmparr = pivot;
		}

		if ((lo - firstIndex) > (lastIndex - hi))
		{
			Sort(arr, hi+1, lastIndex);
			hi = lastIndex = lo - 1;
			lo = firstIndex;
		}
		else
		{
			Sort(arr, firstIndex, lo-1);
			lo = firstIndex = hi + 1;
			hi = lastIndex;
		}
	}
}

void Data::Sort::QuickBubbleSort::Sort(Single *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt lo = firstIndex;
	OSInt hi = lastIndex;
	Single pivot;
	Single tmp;
	Single *tmparr;

	while (hi > lo)
	{
		if ((hi - lo) <= 6)
		{
			BubbleSort_SortSingle(arr, lo, hi);
			return;
		}

		pivot = arr[(lo + hi) >> 1];
		arr[(lo + hi) >> 1] = arr[hi];
		arr[hi--] = pivot;

		while( lo <= hi )
		{
			tmparr = &arr[lo];
			while (*tmparr++ <= pivot)
				if ((++lo) >= hi)
					break;
			tmparr = &arr[hi];
			while (pivot <= *tmparr--)
				if (lo >= (--hi))
					break;
			if (lo < hi)
			{
				tmp = arr[lo];
				arr[lo] = arr[hi];
				arr[hi] = tmp;
			}
		}
		tmparr = &arr[hi];
		if (hi < firstIndex || *tmparr < pivot)
		{
			arr[lastIndex] = *++tmparr;
			*tmparr = pivot;
		}
		else
		{
			arr[lastIndex] = *tmparr;
			*tmparr = pivot;
		}

		if ((lo - firstIndex) > (lastIndex - hi))
		{
			Sort(arr, hi+1, lastIndex);
			hi = lastIndex = lo - 1;
			lo = firstIndex;
		}
		else
		{
			Sort(arr, firstIndex, lo-1);
			lo = firstIndex = hi + 1;
			hi = lastIndex;
		}
	}
}

void Data::Sort::QuickBubbleSort::Sort(Data::IComparable **arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt lo = firstIndex;
	OSInt hi = lastIndex;
	Data::IComparable *pivot;
	Data::IComparable *tmp;
	Data::IComparable **tmparr;

	while (hi > lo)
	{
		if ((hi - lo) <= 6)
		{
			BubbleSort_SortCmpO(arr, lo, hi);
			return;
		}

		pivot = arr[(lo + hi) >> 1];
		arr[(lo + hi) >> 1] = arr[hi];
		arr[hi--] = pivot;

		while( lo <= hi )
		{
			tmparr = &arr[lo];
			while ((*tmparr++)->CompareTo(pivot) <= 0)
				if ((++lo) >= hi)
					break;
			tmparr = &arr[hi];
			while (pivot->CompareTo(*tmparr--) <= 0)
				if (lo >= (--hi))
					break;
			if (lo < hi)
			{
				tmp = arr[lo];
				arr[lo] = arr[hi];
				arr[hi] = tmp;
			}
		}
		tmparr = &arr[hi];
		if (hi < firstIndex || (*tmparr)->CompareTo(pivot) < 0)
		{
			arr[lastIndex] = *++tmparr;
			*tmparr = pivot;
		}
		else
		{
			arr[lastIndex] = *tmparr;
			*tmparr = pivot;
		}

		if ((lo - firstIndex) > (lastIndex - hi))
		{
			Sort(arr, hi+1, lastIndex);
			hi = lastIndex = lo - 1;
			lo = firstIndex;
		}
		else
		{
			Sort(arr, firstIndex, lo-1);
			lo = firstIndex = hi + 1;
			hi = lastIndex;
		}
	}
}

void Data::Sort::QuickBubbleSort::Sort(Int64 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt lo = firstIndex;
	OSInt hi = lastIndex;
	Int64 pivot;
	Int64 tmp;
	Int64 *tmparr;

	while (hi > lo)
	{
		if ((hi - lo) <= 6)
		{
			BubbleSort_SortInt64(arr, lo, hi);
			return;
		}

		pivot = arr[(lo + hi) >> 1];
		arr[(lo + hi) >> 1] = arr[hi];
		arr[hi--] = pivot;

		while( lo <= hi )
		{
			tmparr = &arr[lo];
			while (*tmparr++ <= pivot)
				if ((++lo) >= hi)
					break;
			tmparr = &arr[hi];
			while (pivot <= *tmparr--)
				if (lo >= (--hi))
					break;
			if (lo < hi)
			{
				tmp = arr[lo];
				arr[lo] = arr[hi];
				arr[hi] = tmp;
			}
		}
		tmparr = &arr[hi];
		if (hi < firstIndex || *tmparr < pivot)
		{
			arr[lastIndex] = *++tmparr;
			*tmparr = pivot;
		}
		else
		{
			arr[lastIndex] = *tmparr;
			*tmparr = pivot;
		}

		if ((lo - firstIndex) > (lastIndex - hi))
		{
			Sort(arr, hi+1, lastIndex);
			hi = lastIndex = lo - 1;
			lo = firstIndex;
		}
		else
		{
			Sort(arr, firstIndex, lo-1);
			lo = firstIndex = hi + 1;
			hi = lastIndex;
		}
	}
}

void Data::Sort::QuickBubbleSort::Sort(void **arr, Data::IComparable::CompareFunc func, OSInt firstIndex, OSInt lastIndex)
{
	OSInt lo = firstIndex;
	OSInt hi = lastIndex;
	void *pivot;
	void *tmp;
	void **tmparr;

	while (hi > lo)
	{
		if ((hi - lo) <= 6)
		{
			BubbleSort_SortCmp(arr, func, lo, hi);
			return;
		}

		pivot = arr[(lo + hi) >> 1];
		arr[(lo + hi) >> 1] = arr[hi];
		arr[hi--] = pivot;

		while( lo <= hi )
		{
			tmparr = &arr[lo];
			while (func(*tmparr++, pivot) <= 0)
				if ((++lo) >= hi)
					break;
			tmparr = &arr[hi];
			while (func(pivot, *tmparr--) <= 0)
				if (lo >= (--hi))
					break;
			if (lo < hi)
			{
				tmp = arr[lo];
				arr[lo] = arr[hi];
				arr[hi] = tmp;
			}
		}
		tmparr = &arr[hi];
		if (hi < firstIndex || func(*tmparr, pivot) < 0)
		{
			arr[lastIndex] = *++tmparr;
			*tmparr = pivot;
		}
		else
		{
			arr[lastIndex] = *tmparr;
			*tmparr = pivot;
		}

		if ((lo - firstIndex) > (lastIndex - hi))
		{
			Sort(arr, func, hi+1, lastIndex);
			hi = lastIndex = lo - 1;
			lo = firstIndex;
		}
		else
		{
			Sort(arr, func, firstIndex, lo-1);
			lo = firstIndex = hi + 1;
			hi = lastIndex;
		}
	}
}

void Data::Sort::QuickBubbleSort::SortInv(Int32 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt lo = firstIndex;
	OSInt hi = lastIndex;
	Int32 pivot;
	Int32 tmp;
	Int32 *tmparr;

	while (hi > lo)
	{
		if ((hi - lo) <= 6)
		{
			BubbleSort_SortInt32Inv(arr, lo, hi);
			return;
		}

		pivot = arr[(lo + hi) >> 1];
		arr[(lo + hi) >> 1] = arr[hi];
		arr[hi--] = pivot;

		while( lo <= hi )
		{
			tmparr = &arr[lo];
			while (*tmparr++ >= pivot)
				if ((++lo) >= hi)
					break;
			tmparr = &arr[hi];
			while (pivot >= *tmparr--)
				if (lo >= (--hi))
					break;
			if (lo < hi)
			{
				tmp = arr[lo];
				arr[lo] = arr[hi];
				arr[hi] = tmp;
			}
		}
		tmparr = &arr[hi];
		if (hi < firstIndex || *tmparr > pivot)
		{
			arr[lastIndex] = *++tmparr;
			*tmparr = pivot;
		}
		else
		{
			arr[lastIndex] = *tmparr;
			*tmparr = pivot;
		}

		if ((lo - firstIndex) > (lastIndex - hi))
		{
			SortInv(arr, hi+1, lastIndex);
			hi = lastIndex = lo - 1;
			lo = firstIndex;
		}
		else
		{
			SortInv(arr, firstIndex, lo-1);
			lo = firstIndex = hi + 1;
			hi = lastIndex;
		}
	}
}

void Data::Sort::QuickBubbleSort::SortInv(UInt32 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt lo = firstIndex;
	OSInt hi = lastIndex;
	UInt32 pivot;
	UInt32 tmp;
	UInt32 *tmparr;

	while (hi > lo)
	{
		if ((hi - lo) <= 6)
		{
			BubbleSort_SortUInt32Inv(arr, lo, hi);
			return;
		}

		pivot = arr[(lo + hi) >> 1];
		arr[(lo + hi) >> 1] = arr[hi];
		arr[hi--] = pivot;

		while( lo <= hi )
		{
			tmparr = &arr[lo];
			while (*tmparr++ >= pivot)
				if ((++lo) >= hi)
					break;
			tmparr = &arr[hi];
			while (pivot >= *tmparr--)
				if (lo >= (--hi))
					break;
			if (lo < hi)
			{
				tmp = arr[lo];
				arr[lo] = arr[hi];
				arr[hi] = tmp;
			}
		}
		tmparr = &arr[hi];
		if (hi < firstIndex || *tmparr > pivot)
		{
			arr[lastIndex] = *++tmparr;
			*tmparr = pivot;
		}
		else
		{
			arr[lastIndex] = *tmparr;
			*tmparr = pivot;
		}

		if ((lo - firstIndex) > (lastIndex - hi))
		{
			SortInv(arr, hi+1, lastIndex);
			hi = lastIndex = lo - 1;
			lo = firstIndex;
		}
		else
		{
			SortInv(arr, firstIndex, lo-1);
			lo = firstIndex = hi + 1;
			hi = lastIndex;
		}
	}
}

void Data::Sort::QuickBubbleSort::SortInv(Double *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt lo = firstIndex;
	OSInt hi = lastIndex;
	Double pivot;
	Double tmp;
	Double *tmparr;

	while (hi > lo)
	{
		if ((hi - lo) <= 6)
		{
			BubbleSort_SortDoubleInv(arr, lo, hi);
			return;
		}

		pivot = arr[(lo + hi) >> 1];
		arr[(lo + hi) >> 1] = arr[hi];
		arr[hi--] = pivot;

		while( lo <= hi )
		{
			tmparr = &arr[lo];
			while (*tmparr++ >= pivot)
				if ((++lo) >= hi)
					break;
			tmparr = &arr[hi];
			while (pivot >= *tmparr--)
				if (lo >= (--hi))
					break;
			if (lo < hi)
			{
				tmp = arr[lo];
				arr[lo] = arr[hi];
				arr[hi] = tmp;
			}
		}
		tmparr = &arr[hi];
		if (hi < firstIndex || *tmparr > pivot)
		{
			arr[lastIndex] = *++tmparr;
			*tmparr = pivot;
		}
		else
		{
			arr[lastIndex] = *tmparr;
			*tmparr = pivot;
		}

		if ((lo - firstIndex) > (lastIndex - hi))
		{
			SortInv(arr, hi+1, lastIndex);
			hi = lastIndex = lo - 1;
			lo = firstIndex;
		}
		else
		{
			SortInv(arr, firstIndex, lo-1);
			lo = firstIndex = hi + 1;
			hi = lastIndex;
		}
	}
}

void Data::Sort::QuickBubbleSort::SortInv(Single *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt lo = firstIndex;
	OSInt hi = lastIndex;
	Single pivot;
	Single tmp;
	Single *tmparr;

	while (hi > lo)
	{
		if ((hi - lo) <= 6)
		{
			BubbleSort_SortSingleInv(arr, lo, hi);
			return;
		}

		pivot = arr[(lo + hi) >> 1];
		arr[(lo + hi) >> 1] = arr[hi];
		arr[hi--] = pivot;

		while( lo <= hi )
		{
			tmparr = &arr[lo];
			while (*tmparr++ >= pivot)
				if ((++lo) >= hi)
					break;
			tmparr = &arr[hi];
			while (pivot >= *tmparr--)
				if (lo >= (--hi))
					break;
			if (lo < hi)
			{
				tmp = arr[lo];
				arr[lo] = arr[hi];
				arr[hi] = tmp;
			}
		}
		tmparr = &arr[hi];
		if (hi < firstIndex || *tmparr > pivot)
		{
			arr[lastIndex] = *++tmparr;
			*tmparr = pivot;
		}
		else
		{
			arr[lastIndex] = *tmparr;
			*tmparr = pivot;
		}

		if ((lo - firstIndex) > (lastIndex - hi))
		{
			SortInv(arr, hi+1, lastIndex);
			hi = lastIndex = lo - 1;
			lo = firstIndex;
		}
		else
		{
			SortInv(arr, firstIndex, lo-1);
			lo = firstIndex = hi + 1;
			hi = lastIndex;
		}
	}
}

void Data::Sort::QuickBubbleSort::SortInv(Data::IComparable **arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt lo = firstIndex;
	OSInt hi = lastIndex;
	Data::IComparable *pivot;
	Data::IComparable *tmp;
	Data::IComparable **tmparr;

	while (hi > lo)
	{
		if ((hi - lo) <= 6)
		{
			BubbleSort_SortCmpOInv(arr, lo, hi);
			return;
		}

		pivot = arr[(lo + hi) >> 1];
		arr[(lo + hi) >> 1] = arr[hi];
		arr[hi--] = pivot;

		while( lo <= hi )
		{
			tmparr = &arr[lo];
			while ((*tmparr++)->CompareTo(pivot) >= 0)
				if ((++lo) >= hi)
					break;
			tmparr = &arr[hi];
			while (pivot->CompareTo(*tmparr--) >= 0)
				if (lo >= (--hi))
					break;
			if (lo < hi)
			{
				tmp = arr[lo];
				arr[lo] = arr[hi];
				arr[hi] = tmp;
			}
		}
		tmparr = &arr[hi];
		if (hi < firstIndex || (*tmparr)->CompareTo(pivot) > 0)
		{
			arr[lastIndex] = *++tmparr;
			*tmparr = pivot;
		}
		else
		{
			arr[lastIndex] = *tmparr;
			*tmparr = pivot;
		}

		if ((lo - firstIndex) > (lastIndex - hi))
		{
			SortInv(arr, hi+1, lastIndex);
			hi = lastIndex = lo - 1;
			lo = firstIndex;
		}
		else
		{
			SortInv(arr, firstIndex, lo-1);
			lo = firstIndex = hi + 1;
			hi = lastIndex;
		}
	}
}

void Data::Sort::QuickBubbleSort::SortInv(Int64 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt lo = firstIndex;
	OSInt hi = lastIndex;
	Int64 pivot;
	Int64 tmp;
	Int64 *tmparr;

	while (hi > lo)
	{
		if ((hi - lo) <= 6)
		{
			BubbleSort_SortInt64Inv(arr, lo, hi);
			return;
		}

		pivot = arr[(lo + hi) >> 1];
		arr[(lo + hi) >> 1] = arr[hi];
		arr[hi--] = pivot;

		while( lo <= hi )
		{
			tmparr = &arr[lo];
			while (*tmparr++ >= pivot)
				if ((++lo) >= hi)
					break;
			tmparr = &arr[hi];
			while (pivot >= *tmparr--)
				if (lo >= (--hi))
					break;
			if (lo < hi)
			{
				tmp = arr[lo];
				arr[lo] = arr[hi];
				arr[hi] = tmp;
			}
		}
		tmparr = &arr[hi];
		if (hi < firstIndex || *tmparr > pivot)
		{
			arr[lastIndex] = *++tmparr;
			*tmparr = pivot;
		}
		else
		{
			arr[lastIndex] = *tmparr;
			*tmparr = pivot;
		}

		if ((lo - firstIndex) > (lastIndex - hi))
		{
			SortInv(arr, hi+1, lastIndex);
			hi = lastIndex = lo - 1;
			lo = firstIndex;
		}
		else
		{
			SortInv(arr, firstIndex, lo-1);
			lo = firstIndex = hi + 1;
			hi = lastIndex;
		}
	}
}

void Data::Sort::QuickBubbleSort::SortInv(void **arr, Data::IComparable::CompareFunc func, OSInt firstIndex, OSInt lastIndex)
{
	OSInt lo = firstIndex;
	OSInt hi = lastIndex;
	void *pivot;
	void *tmp;
	void **tmparr;

	while (hi > lo)
	{
		if ((hi - lo) <= 6)
		{
			BubbleSort_SortCmpInv(arr, func, lo, hi);
			return;
		}

		pivot = arr[(lo + hi) >> 1];
		arr[(lo + hi) >> 1] = arr[hi];
		arr[hi--] = pivot;

		while( lo <= hi )
		{
			tmparr = &arr[lo];
			while (func(*tmparr++, pivot) >= 0)
				if ((++lo) >= hi)
					break;
			tmparr = &arr[hi];
			while (func(pivot, *tmparr--) >= 0)
				if (lo >= (--hi))
					break;
			if (lo < hi)
			{
				tmp = arr[lo];
				arr[lo] = arr[hi];
				arr[hi] = tmp;
			}
		}
		tmparr = &arr[hi];
		if (hi < firstIndex || func(*tmparr, pivot) > 0)
		{
			arr[lastIndex] = *++tmparr;
			*tmparr = pivot;
		}
		else
		{
			arr[lastIndex] = *tmparr;
			*tmparr = pivot;
		}

		if ((lo - firstIndex) > (lastIndex - hi))
		{
			SortInv(arr, func, hi+1, lastIndex);
			hi = lastIndex = lo - 1;
			lo = firstIndex;
		}
		else
		{
			SortInv(arr, func, firstIndex, lo-1);
			lo = firstIndex = hi + 1;
			hi = lastIndex;
		}
	}
}
