#ifndef _SM_DATA_SORT_INSERTIONSORT
#define _SM_DATA_SORT_INSERTIONSORT
#include "Data/IComparable.h"

extern "C"
{
	// Binary Search for larger arr
	void InsertionSort_SortBInt32(Int32 *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortBStr(UTF8Char **arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortBCmp(void **arr, Data::IComparable::CompareFunc func, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortBCmpO(Data::IComparable **arr, OSInt firstIndex, OSInt lastIndex);

	// Sequencial Search for small arr
	void InsertionSort_SortInt32(Int32 *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortUInt32(UInt32 *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortInt64(Int64 *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortSingle(Single *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortDouble(Double *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortStr(UTF8Char **arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortCmp(void **arr, Data::IComparable::CompareFunc func, OSInt firstIndex, OSInt lastIndex);

	void InsertionSort_SortInt32Inv(Int32 *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortUInt32Inv(UInt32 *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortInt64Inv(Int64 *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortSingleInv(Single *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortDoubleInv(Double *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortCmpInv(void **arr, Data::IComparable::CompareFunc func, OSInt firstIndex, OSInt lastIndex);
}

#endif
