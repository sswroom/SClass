#ifndef _SM_DATA_SORT_ARTIFICIALQUICKSORTC
#define _SM_DATA_SORT_ARTIFICIALQUICKSORTC
#include "Data/IComparable.h"

//http://www.critticall.com/ArtificialSort.html

extern "C"
{
	void ArtificialQuickSort_PreSortInt32(Int32 *arr, OSInt firstIndex, OSInt lastIndex);
	void ArtificialQuickSort_PreSortUInt32(UInt32 *arr, OSInt firstIndex, OSInt lastIndex);
	void ArtificialQuickSort_PreSortInt64(Int64 *arr, OSInt firstIndex, OSInt lastIndex);
	void ArtificialQuickSort_PreSortSingle(Single *arr, OSInt firstIndex, OSInt lastIndex);
	void ArtificialQuickSort_PreSortDouble(Double *arr, OSInt firstIndex, OSInt lastIndex);
	void ArtificialQuickSort_PreSortStr(UTF8Char **arr, OSInt firstIndex, OSInt lastIndex);

	void ArtificialQuickSort_PreSortInt32Inv(Int32 *arr, OSInt firstIndex, OSInt lastIndex);
	void ArtificialQuickSort_PreSortUInt32Inv(UInt32 *arr, OSInt firstIndex, OSInt lastIndex);
	void ArtificialQuickSort_PreSortInt64Inv(Int64 *arr, OSInt firstIndex, OSInt lastIndex);
	void ArtificialQuickSort_PreSortSingleInv(Single *arr, OSInt firstIndex, OSInt lastIndex);
	void ArtificialQuickSort_PreSortDoubleInv(Double *arr, OSInt firstIndex, OSInt lastIndex);

	void ArtificialQuickSort_SortInt32(Int32 *arr, OSInt firstIndex, OSInt lastIndex);
	void ArtificialQuickSort_SortUInt32(UInt32 *arr, OSInt firstIndex, OSInt lastIndex);
	void ArtificialQuickSort_SortInt64(Int64 *arr, OSInt firstIndex, OSInt lastIndex);
	void ArtificialQuickSort_SortSingle(Single *arr, OSInt firstIndex, OSInt lastIndex);
	void ArtificialQuickSort_SortDouble(Double *arr, OSInt firstIndex, OSInt lastIndex);
	void ArtificialQuickSort_SortStr(UTF8Char **arr, OSInt firstIndex, OSInt lastIndex);

	void ArtificialQuickSort_SortInt32Inv(Int32 *arr, OSInt firstIndex, OSInt lastIndex);
	void ArtificialQuickSort_SortUInt32Inv(UInt32 *arr, OSInt firstIndex, OSInt lastIndex);
	void ArtificialQuickSort_SortInt64Inv(Int64 *arr, OSInt firstIndex, OSInt lastIndex);
	void ArtificialQuickSort_SortSingleInv(Single *arr, OSInt firstIndex, OSInt lastIndex);
	void ArtificialQuickSort_SortDoubleInv(Double *arr, OSInt firstIndex, OSInt lastIndex);
}
#endif
