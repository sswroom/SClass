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

		virtual UOSInt SortedInsert(UInt32 Val);
		virtual OSInt SortedIndexOf(UInt32 Val);
	};
}
#endif
