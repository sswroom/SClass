#ifndef _SM_DATA_SORT_BUBBLESORT_C
#define _SM_DATA_SORT_BUBBLESORT_C
extern "C"
{
	void BubbleSort_SortInt32(Int32 *arr, IntOS firstIndex, IntOS lastIndex);
	void BubbleSort_SortUInt32(UInt32 *arr, IntOS firstIndex, IntOS lastIndex);
	void BubbleSort_SortInt64(Int64 *arr, IntOS firstIndex, IntOS lastIndex);
	void BubbleSort_SortDouble(Double *arr, IntOS firstIndex, IntOS lastIndex);
	void BubbleSort_SortSingle(Single *arr, IntOS firstIndex, IntOS lastIndex);
	void BubbleSort_SortStrW(WChar **arr, IntOS firstIndex, IntOS lastIndex);
	void BubbleSort_SortStrC(UTF8Char **arr, IntOS firstIndex, IntOS lastIndex);

	void BubbleSort_SortInt32Inv(Int32 *arr, IntOS firstIndex, IntOS lastIndex);
	void BubbleSort_SortUInt32Inv(UInt32 *arr, IntOS firstIndex, IntOS lastIndex);
	void BubbleSort_SortInt64Inv(Int64 *arr, IntOS firstIndex, IntOS lastIndex);
	void BubbleSort_SortDoubleInv(Double *arr, IntOS firstIndex, IntOS lastIndex);
	void BubbleSort_SortSingleInv(Single *arr, IntOS firstIndex, IntOS lastIndex);
	void BubbleSort_SortStrWInv(WChar **arr, IntOS firstIndex, IntOS lastIndex);
	void BubbleSort_SortStrInv(UTF8Char **arr, IntOS firstIndex, IntOS lastIndex);
}
#endif
