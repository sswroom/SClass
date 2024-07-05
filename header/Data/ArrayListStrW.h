#ifndef _SM_DATA_ARRAYLISTSTRW
#define _SM_DATA_ARRAYLISTSTRW
#include "Data/SortableArrayList.h"

namespace Data
{
	class ArrayListStrW : public Data::SortableArrayList<const WChar*>
	{
	public:
		ArrayListStrW();
		ArrayListStrW(UOSInt Capacity);

		virtual NN<Data::ArrayList<const WChar*>> Clone() const;
		virtual OSInt Compare(const WChar* obj1, const WChar* obj2) const;
		UnsafeArray<const WChar> JoinNewStr() const;
	};
}
#endif
