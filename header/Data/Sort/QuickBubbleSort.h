#ifndef _SM_DATA_SORT_QUICKBUBBLESORT
#define _SM_DATA_SORT_QUICKBUBBLESORT
#include "Data/Comparable.h"

namespace Data
{
	namespace Sort
	{
		class QuickBubbleSort
		{
		public:
			static void Sort(UnsafeArray<Int32> arr, OSInt firstIndex, OSInt lastIndex);
			static void Sort(UnsafeArray<UInt32> arr, OSInt firstIndex, OSInt lastIndex);
			static void Sort(UnsafeArray<Double> arr, OSInt firstIndex, OSInt lastIndex);
			static void Sort(UnsafeArray<Single> arr, OSInt firstIndex, OSInt lastIndex);
			static void Sort(Data::Comparable **arr, OSInt firstIndex, OSInt lastIndex);
			static void Sort(UnsafeArray<Int64> arr, OSInt firstIndex, OSInt lastIndex);
			static void SortInv(UnsafeArray<Int32> arr, OSInt firstIndex, OSInt lastIndex);
			static void SortInv(UnsafeArray<UInt32> arr, OSInt firstIndex, OSInt lastIndex);
			static void SortInv(UnsafeArray<Double> arr, OSInt firstIndex, OSInt lastIndex);
			static void SortInv(UnsafeArray<Single> arr, OSInt firstIndex, OSInt lastIndex);
			static void SortInv(Data::Comparable **arr, OSInt firstIndex, OSInt lastIndex);
			static void SortInv(UnsafeArray<Int64> arr, OSInt firstIndex, OSInt lastIndex);
		};
	}
}
#endif
