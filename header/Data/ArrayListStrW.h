#ifndef _SM_DATA_ARRAYLISTSTRW
#define _SM_DATA_ARRAYLISTSTRW
#include "Data/SortableArrayList.hpp"

namespace Data
{
	class ArrayListStrW : public Data::SortableArrayList<const WChar*>
	{
	public:
		ArrayListStrW();
		ArrayListStrW(UIntOS Capacity);

		virtual NN<Data::ArrayList<const WChar*>> Clone() const;
		virtual IntOS Compare(const WChar* obj1, const WChar* obj2) const;
		UnsafeArray<const WChar> JoinNewStr() const;
	};
}
#endif
