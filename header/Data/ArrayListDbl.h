#ifndef _SM_DATA_ARRAYLISTDBL
#define _SM_DATA_ARRAYLISTDBL
#include "Data/SortableArrayList.h"

namespace Data
{
	class ArrayListDbl : public Data::SortableArrayList<Double>
	{
	public:
		ArrayListDbl();
		ArrayListDbl(UOSInt capacity);

		virtual ArrayList<Double> *Clone();
		virtual OSInt CompareItem(Double obj1, Double obj2);
	};
}
#endif
