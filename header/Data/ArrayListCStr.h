#ifndef _SM_DATA_ARRAYLISTCSTR
#define _SM_DATA_ARRAYLISTCSTR
#include "Data/SortableArrayList.hpp"
#include "Text/CString.h"

namespace Data
{
	class ArrayListCStr : public Data::SortableArrayList<Text::CStringNN>
	{
	public:
		ArrayListCStr();
		ArrayListCStr(UIntOS capacity);

		virtual NN<Data::ArrayList<Text::CStringNN>> Clone() const;
		virtual IntOS Compare(Text::CStringNN obj1, Text::CStringNN obj2) const;
	};
}
#endif
