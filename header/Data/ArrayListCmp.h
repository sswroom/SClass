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
		ArrayListCmp(UOSInt Capacity);

		virtual Data::ArrayList<Data::IComparable*> *Clone() const;
		virtual OSInt CompareItem(Data::IComparable* obj1, Data::IComparable* obj2) const;
	};
}
#endif
