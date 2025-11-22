#ifndef _SM_DATA_ARRAYLISTCMP
#define _SM_DATA_ARRAYLISTCMP
#include "Data/Comparable.h"
#include "Data/SortableArrayListNN.hpp"

namespace Data
{
	class ArrayListCmp : public Data::SortableArrayListNN<Data::Comparable>
	{
	public:
		ArrayListCmp();
		ArrayListCmp(UOSInt Capacity);

		virtual NN<Data::ArrayListNN<Data::Comparable>> Clone() const;
		virtual OSInt Compare(NN<Data::Comparable> obj1, NN<Data::Comparable> obj2) const;
	};
}
#endif
