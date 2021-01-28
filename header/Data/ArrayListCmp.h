#ifndef _SM_DATA_ARRAYLISTCMP
#define _SM_DATA_ARRAYLISTCMP
#include "Data/SortableArrayList.h"
#include "Data/IComparable.h"

namespace Data
{
	class ArrayListCmp : public Data::SortableArrayList<Data::IComparable*>
	{
	public:
		ArrayListCmp();
		ArrayListCmp(OSInt Capacity);

		virtual Data::ArrayList<Data::IComparable*> *Clone();

		virtual UOSInt SortedInsert(Data::IComparable *val);
		virtual OSInt SortedIndexOf(Data::IComparable *val);
	};
}
#endif
