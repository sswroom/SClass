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
			static void Sort(UnsafeArray<Int32> arr, IntOS firstIndex, IntOS lastIndex);
			static void Sort(UnsafeArray<UInt32> arr, IntOS firstIndex, IntOS lastIndex);
			static void Sort(UnsafeArray<Double> arr, IntOS firstIndex, IntOS lastIndex);
			static void Sort(UnsafeArray<Single> arr, IntOS firstIndex, IntOS lastIndex);
			static void Sort(UnsafeArray<NN<Data::Comparable>> arr, IntOS firstIndex, IntOS lastIndex);
			static void Sort(UnsafeArray<Int64> arr, IntOS firstIndex, IntOS lastIndex);
			static void SortInv(UnsafeArray<Int32> arr, IntOS firstIndex, IntOS lastIndex);
			static void SortInv(UnsafeArray<UInt32> arr, IntOS firstIndex, IntOS lastIndex);
			static void SortInv(UnsafeArray<Double> arr, IntOS firstIndex, IntOS lastIndex);
			static void SortInv(UnsafeArray<Single> arr, IntOS firstIndex, IntOS lastIndex);
			static void SortInv(UnsafeArray<NN<Data::Comparable>> arr, IntOS firstIndex, IntOS lastIndex);
			static void SortInv(UnsafeArray<Int64> arr, IntOS firstIndex, IntOS lastIndex);
		};
	}
}
#endif
