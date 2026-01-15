#ifndef _SM_DATA_SORT_INSERTIONSORT_C
#define _SM_DATA_SORT_INSERTIONSORT_C
extern "C"
{
	// Binary Search for larger arr
	void InsertionSort_SortBInt32(Int32 *arr, IntOS firstIndex, IntOS lastIndex);
	void InsertionSort_SortBStr(UTF8Char **arr, IntOS firstIndex, IntOS lastIndex);

	// Sequencial Search for small arr
	void InsertionSort_SortInt32(Int32 *arr, IntOS firstIndex, IntOS lastIndex);
	void InsertionSort_SortUInt32(UInt32 *arr, IntOS firstIndex, IntOS lastIndex);
	void InsertionSort_SortInt64(Int64 *arr, IntOS firstIndex, IntOS lastIndex);
	void InsertionSort_SortSingle(Single *arr, IntOS firstIndex, IntOS lastIndex);
	void InsertionSort_SortDouble(Double *arr, IntOS firstIndex, IntOS lastIndex);
	void InsertionSort_SortStr(UTF8Char **arr, IntOS firstIndex, IntOS lastIndex);

	void InsertionSort_SortInt32Inv(Int32 *arr, IntOS firstIndex, IntOS lastIndex);
	void InsertionSort_SortUInt32Inv(UInt32 *arr, IntOS firstIndex, IntOS lastIndex);
	void InsertionSort_SortInt64Inv(Int64 *arr, IntOS firstIndex, IntOS lastIndex);
	void InsertionSort_SortSingleInv(Single *arr, IntOS firstIndex, IntOS lastIndex);
	void InsertionSort_SortDoubleInv(Double *arr, IntOS firstIndex, IntOS lastIndex);
}
#endif
