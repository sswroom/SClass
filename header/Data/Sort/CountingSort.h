#ifndef _SM_DATA_SORT_COUNTINGSORT
#define _SM_DATA_SORT_COUNTINGSORT
#include "Data/Comparable.h"

namespace Data
{
	namespace Sort
	{
		class CountingSort
		{
		private:
			Int32 minVal;
			Int32 maxVal;
			UnsafeArray<Int32> valCount;
		public:
			CountingSort(Int32 minVal, Int32 maxVal);
			~CountingSort();

			void SortInt32(UnsafeArray<const Int32> inputArr, UnsafeArray<Int32> outputArr, IntOS firstIndex, IntOS lastIndex);
		};
	}
}
#endif
