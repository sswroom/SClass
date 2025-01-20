#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Comparable.h"
#include "Data/Sort/QuickBubbleSort.h"
#include "Data/Sort/BubbleSort.h"

void Data::Sort::QuickBubbleSort::Sort(UnsafeArray<Int32> arr, OSInt firstIndex, OSInt lastIndex)
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
			BubbleSort_SortInt32(arr.Ptr(), lo, hi);
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

void Data::Sort::QuickBubbleSort::Sort(UnsafeArray<UInt32> arr, OSInt firstIndex, OSInt lastIndex)
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
			BubbleSort_SortUInt32(arr.Ptr(), lo, hi);
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

void Data::Sort::QuickBubbleSort::Sort(UnsafeArray<Double> arr, OSInt firstIndex, OSInt lastIndex)
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
			BubbleSort_SortDouble(arr.Ptr(), lo, hi);
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

void Data::Sort::QuickBubbleSort::Sort(UnsafeArray<Single> arr, OSInt firstIndex, OSInt lastIndex)
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
			BubbleSort_SortSingle(arr.Ptr(), lo, hi);
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

void Data::Sort::QuickBubbleSort::Sort(Data::Comparable **arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt lo = firstIndex;
	OSInt hi = lastIndex;
	Data::Comparable *pivot;
	Data::Comparable *tmp;
	Data::Comparable **tmparr;

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

void Data::Sort::QuickBubbleSort::Sort(UnsafeArray<Int64> arr, OSInt firstIndex, OSInt lastIndex)
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
			BubbleSort_SortInt64(arr.Ptr(), lo, hi);
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

void Data::Sort::QuickBubbleSort::SortInv(UnsafeArray<Int32> arr, OSInt firstIndex, OSInt lastIndex)
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
			BubbleSort_SortInt32Inv(arr.Ptr(), lo, hi);
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

void Data::Sort::QuickBubbleSort::SortInv(UnsafeArray<UInt32> arr, OSInt firstIndex, OSInt lastIndex)
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
			BubbleSort_SortUInt32Inv(arr.Ptr(), lo, hi);
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

void Data::Sort::QuickBubbleSort::SortInv(UnsafeArray<Double> arr, OSInt firstIndex, OSInt lastIndex)
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
			BubbleSort_SortDoubleInv(arr.Ptr(), lo, hi);
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

void Data::Sort::QuickBubbleSort::SortInv(UnsafeArray<Single> arr, OSInt firstIndex, OSInt lastIndex)
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
			BubbleSort_SortSingleInv(arr.Ptr(), lo, hi);
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

void Data::Sort::QuickBubbleSort::SortInv(Data::Comparable **arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt lo = firstIndex;
	OSInt hi = lastIndex;
	Data::Comparable *pivot;
	Data::Comparable *tmp;
	Data::Comparable **tmparr;

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

void Data::Sort::QuickBubbleSort::SortInv(UnsafeArray<Int64> arr, OSInt firstIndex, OSInt lastIndex)
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
			BubbleSort_SortInt64Inv(arr.Ptr(), lo, hi);
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
