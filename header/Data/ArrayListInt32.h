#ifndef _SM_DATA_ARRAYLISTINT32
#define _SM_DATA_ARRAYLISTINT32
#include "Data/SortableArrayList.h"

namespace Data
{
	class ArrayListInt32 : public Data::SortableArrayList<Int32>
	{
	public:
		ArrayListInt32();
		ArrayListInt32(OSInt Capacity);

		virtual ArrayList<Int32> *Clone();

		virtual UOSInt SortedInsert(Int32 Val);
		virtual OSInt SortedIndexOf(Int32 Val);
	};
}
#endif
