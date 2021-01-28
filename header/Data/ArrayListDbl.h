#ifndef _SM_DATA_ARRAYLISTDBL
#define _SM_DATA_ARRAYLISTDBL
#include "Data/SortableArrayList.h"

namespace Data
{
	class ArrayListDbl : public Data::SortableArrayList<Double>
	{
	public:
		ArrayListDbl();
		ArrayListDbl(OSInt Capacity);

		virtual ArrayList<Double> *Clone();
		virtual UOSInt SortedInsert(Double Val);
		virtual OSInt SortedIndexOf(Double Val);
	};
}
#endif
