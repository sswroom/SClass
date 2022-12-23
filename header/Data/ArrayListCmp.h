#ifndef _SM_DATA_ARRAYLISTCMP
#define _SM_DATA_ARRAYLISTCMP
#include "Data/IComparable.h"
#include "Data/SortableArrayList.h"

namespace Data
{
	class ArrayListCmp : public Data::SortableArrayList<Data::IComparable*>
	{
	public:
		ArrayListCmp();
		ArrayListCmp(UOSInt Capacity);

		virtual Data::ArrayList<Data::IComparable*> *Clone() const;
		virtual OSInt Compare(Data::IComparable* obj1, Data::IComparable* obj2) const;
	};
}
#endif
