#ifndef _SM_DATA_ARRAYLISTINT32
#define _SM_DATA_ARRAYLISTINT32
#include "Data/SortableArrayList.h"

namespace Data
{
	class ArrayListInt32 : public Data::SortableArrayList<Int32>
	{
	public:
		ArrayListInt32();
		ArrayListInt32(UOSInt capacity);

		virtual ArrayList<Int32> *Clone();
		virtual OSInt CompareItem(Int32 obj1, Int32 obj2);
	};
}
#endif
