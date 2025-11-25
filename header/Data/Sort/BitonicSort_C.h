#ifndef _SM_DATA_SORT_BITONICSORTC
#define _SM_DATA_SORT_BITONICSORTC

extern "C"
{
	void BitonicSort_SortInt32(Int32 *arr, OSInt firstIndex, OSInt lastIndex);
	void BitonicSort_SortUInt32(UInt32 *arr, OSInt firstIndex, OSInt lastIndex);
	void BitonicSort_SortStr(UTF8Char **arr, OSInt firstIndex, OSInt lastIndex);
}

#endif
