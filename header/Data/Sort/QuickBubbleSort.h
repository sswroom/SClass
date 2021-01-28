#ifndef _SM_DATA_SORT_QUICKBUBBLESORT
#define _SM_DATA_SORT_QUICKBUBBLESORT
#include "Data/IComparable.h"

namespace Data
{
	namespace Sort
	{
		class QuickBubbleSort
		{
		public:
			static void Sort(Int32 *arr, OSInt firstIndex, OSInt lastIndex);
			static void Sort(UInt32 *arr, OSInt firstIndex, OSInt lastIndex);
			static void Sort(Double *arr, OSInt firstIndex, OSInt lastIndex);
			static void Sort(Single *arr, OSInt firstIndex, OSInt lastIndex);
			static void Sort(Data::IComparable **arr, OSInt firstIndex, OSInt lastIndex);
			static void Sort(Int64 *arr, OSInt firstIndex, OSInt lastIndex);
			static void Sort(void **arr, Data::IComparable::CompareFunc func, OSInt firstIndex, OSInt lastIndex);
			static void SortInv(Int32 *arr, OSInt firstIndex, OSInt lastIndex);
			static void SortInv(UInt32 *arr, OSInt firstIndex, OSInt lastIndex);
			static void SortInv(Double *arr, OSInt firstIndex, OSInt lastIndex);
			static void SortInv(Single *arr, OSInt firstIndex, OSInt lastIndex);
			static void SortInv(Data::IComparable **arr, OSInt firstIndex, OSInt lastIndex);
			static void SortInv(Int64 *arr, OSInt firstIndex, OSInt lastIndex);
			static void SortInv(void **arr, Data::IComparable::CompareFunc func, OSInt firstIndex, OSInt lastIndex);
		};
	}
}
#endif
