#ifndef _SM_DATA_ARRAYLISTSNG
#define _SM_DATA_ARRAYLISTSNG
#include "Data/SortableArrayList.h"

namespace Data
{
	class ArrayListSng : public Data::SortableArrayList<Single>
	{
	public:
		ArrayListSng();
		ArrayListSng(OSInt Capacity);

		virtual Data::ArrayList<Single> *Clone();
		virtual OSInt CompareItem(Single obj1, Single obj2);
	};
}
#endif
