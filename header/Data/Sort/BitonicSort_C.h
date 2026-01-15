#ifndef _SM_DATA_SORT_BITONICSORTC
#define _SM_DATA_SORT_BITONICSORTC

extern "C"
{
	void BitonicSort_SortInt32(Int32 *arr, IntOS firstIndex, IntOS lastIndex);
	void BitonicSort_SortUInt32(UInt32 *arr, IntOS firstIndex, IntOS lastIndex);
	void BitonicSort_SortStr(UTF8Char **arr, IntOS firstIndex, IntOS lastIndex);
}

#endif
