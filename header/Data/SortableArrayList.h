#ifndef _SM_DATA_SORTABLEARRAYLIST
#define _SM_DATA_SORTABLEARRAYLIST
#include "Data/ArrayList.h"

namespace Data
{
	template <class T> class SortableArrayList : public Data::ArrayList<T>
	{
	public:
		SortableArrayList() : Data::ArrayList<T>(){};
		SortableArrayList(UOSInt capacity) : Data::ArrayList<T>(capacity){};

		virtual UOSInt SortedInsert(T val) = 0;
		virtual OSInt SortedIndexOf(T val) = 0;
	};
}
#endif
