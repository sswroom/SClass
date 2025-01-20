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
			Int32 *valCount;
		public:
			PigeonholeSort(Int32 minVal, Int32 maxVal);
			~PigeonholeSort();

			void SortInt32(Int32 *arr, OSInt firstIndex, OSInt lastIndex);
		};
	};
};
#endif
