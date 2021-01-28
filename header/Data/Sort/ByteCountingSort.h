#ifndef _SM_DATA_SORT_BYTECOUNTINGSORT
#define _SM_DATA_SORT_BYTECOUNTINGSORT
#include "Data/IComparable.h"

extern "C"
{
	Bool ByteCountingSort_SortUInt32(UInt32 *arr, OSInt firstIndex, OSInt lastIndex);
}
#endif
