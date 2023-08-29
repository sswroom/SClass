#ifndef _SM_DATA_SORT_BUBBLESORT
#define _SM_DATA_SORT_BUBBLESORT
#include "Data/IComparable.h"

extern "C"
{
	void BubbleSort_SortInt32(Int32 *arr, OSInt firstIndex, OSInt lastIndex);
	void BubbleSort_SortUInt32(UInt32 *arr, OSInt firstIndex, OSInt lastIndex);
	void BubbleSort_SortInt64(Int64 *arr, OSInt firstIndex, OSInt lastIndex);
	void BubbleSort_SortDouble(Double *arr, OSInt firstIndex, OSInt lastIndex);
	void BubbleSort_SortSingle(Single *arr, OSInt firstIndex, OSInt lastIndex);
	void BubbleSort_SortStrW(WChar **arr, OSInt firstIndex, OSInt lastIndex);
	void BubbleSort_SortStrC(UTF8Char **arr, OSInt firstIndex, OSInt lastIndex);
	void BubbleSort_SortCmpO(Data::IComparable **arr, OSInt firstIndex, OSInt lastIndex);

	void BubbleSort_SortInt32Inv(Int32 *arr, OSInt firstIndex, OSInt lastIndex);
	void BubbleSort_SortUInt32Inv(UInt32 *arr, OSInt firstIndex, OSInt lastIndex);
	void BubbleSort_SortInt64Inv(Int64 *arr, OSInt firstIndex, OSInt lastIndex);
	void BubbleSort_SortDoubleInv(Double *arr, OSInt firstIndex, OSInt lastIndex);
	void BubbleSort_SortSingleInv(Single *arr, OSInt firstIndex, OSInt lastIndex);
	void BubbleSort_SortStrWInv(WChar **arr, OSInt firstIndex, OSInt lastIndex);
	void BubbleSort_SortStrInv(UTF8Char **arr, OSInt firstIndex, OSInt lastIndex);
	void BubbleSort_SortCmpOInv(Data::IComparable **arr, OSInt firstIndex, OSInt lastIndex);
}
#endif
