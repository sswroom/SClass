#ifndef _SM_DATA_SORT_COUNTINGSORT
#define _SM_DATA_SORT_COUNTINGSORT
#include "Data/IComparable.h"

namespace Data
{
	namespace Sort
	{
		class CountingSort
		{
		private:
			Int32 minVal;
			Int32 maxVal;
			Int32 *valCount;
		public:
			CountingSort(Int32 minVal, Int32 maxVal);
			~CountingSort();

			void SortInt32(const Int32 *inputArr, Int32 *outputArr, OSInt firstIndex, OSInt lastIndex);
		};
	}
}
#endif
