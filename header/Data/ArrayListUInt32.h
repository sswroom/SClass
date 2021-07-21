#ifndef _SM_DATA_ARRAYLISTUINT32
#define _SM_DATA_ARRAYLISTUINT32
#include "Data/SortableArrayList.h"

namespace Data
{
	class ArrayListUInt32 : public Data::SortableArrayList<UInt32>
	{
	public:
		ArrayListUInt32();
		ArrayListUInt32(UOSInt capacity);

		virtual ArrayList<UInt32> *Clone();
		virtual OSInt CompareItem(UInt32 obj1, UInt32 obj2);
	};
}
#endif
