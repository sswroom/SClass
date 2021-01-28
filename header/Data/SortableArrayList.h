#ifndef _SM_DATA_SORTABLEARRAYLIST
#define _SM_DATA_SORTABLEARRAYLIST
#include "Data/ArrayList.h"

namespace Data
{
	template <class T> class SortableArrayList : public Data::ArrayList<T>
	{
	public:
		SortableArrayList() : Data::ArrayList<T>(){};
		SortableArrayList(UOSInt Capacity) : Data::ArrayList<T>(Capacity){};

		virtual UOSInt SortedInsert(T Val) = 0;
		virtual OSInt SortedIndexOf(T Val) = 0;
	};
}
#endif
