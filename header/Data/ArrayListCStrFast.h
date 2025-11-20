#ifndef _SM_DATA_ARRAYLISTCSTRFAST
#define _SM_DATA_ARRAYLISTCSTRFAST
#include "Data/SortableArrayList.hpp"
#include "Text/CString.h"

namespace Data
{
	class ArrayListCStrFast : public Data::SortableArrayList<Text::CStringNN>
	{
	public:
		ArrayListCStrFast();
		ArrayListCStrFast(UOSInt capacity);

		virtual NN<Data::ArrayList<Text::CStringNN>> Clone() const;
		virtual OSInt Compare(Text::CStringNN obj1, Text::CStringNN obj2) const;
	};
}
#endif
