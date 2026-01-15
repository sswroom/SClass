#ifndef _SM_DATA_SORT_PIGEONHOLESORT
#define _SM_DATA_SORT_PIGEONHOLESORT
#include "Data/Comparable.h"

namespace Data
{
	namespace Sort
	{
		class PigeonholeSort
		{
		private:
			Int32 minVal;
			Int32 maxVal;
			UnsafeArray<Int32> valCount;
		public:
			PigeonholeSort(Int32 minVal, Int32 maxVal);
			~PigeonholeSort();

			void SortInt32(UnsafeArray<Int32> arr, IntOS firstIndex, IntOS lastIndex);
		};
	}
}
#endif
