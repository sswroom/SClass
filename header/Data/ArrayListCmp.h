#ifndef _SM_DATA_ARRAYLISTCMP
#define _SM_DATA_ARRAYLISTCMP
#include "Data/IComparable.h"
#include "Data/SortableArrayListNN.h"

namespace Data
{
	class ArrayListCmp : public Data::SortableArrayListNN<Data::IComparable>
	{
	public:
		ArrayListCmp();
		ArrayListCmp(UOSInt Capacity);

		virtual NN<Data::ArrayListNN<Data::IComparable>> Clone() const;
		virtual OSInt Compare(NN<Data::IComparable> obj1, NN<Data::IComparable> obj2) const;
	};
}
#endif
