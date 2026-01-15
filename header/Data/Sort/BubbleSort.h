#ifndef _SM_DATA_SORT_BUBBLESORT
#define _SM_DATA_SORT_BUBBLESORT
#include "Data/Comparable.h"

namespace Data
{
	namespace Sort
	{
		class BubbleSort
		{
		public:
			static void SortCmpO(UnsafeArray<NN<Data::Comparable>> arr, IntOS firstIndex, IntOS lastIndex);
			static void SortCmpOInv(UnsafeArray<NN<Data::Comparable>> arr, IntOS firstIndex, IntOS lastIndex);
		};
	}
}
#endif
