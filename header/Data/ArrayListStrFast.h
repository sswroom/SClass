#ifndef _SM_DATA_ARRAYLISTSTRFAST
#define _SM_DATA_ARRAYLISTSTRFAST
#include "Data/SortableArrayListObj.hpp"
#include "Text/String.h"

namespace Data
{
	class ArrayListStrFast : public Data::SortableArrayListObj<Text::String*>
	{
	public:
		ArrayListStrFast();
		ArrayListStrFast(UOSInt capacity);

		virtual NN<Data::ArrayListObj<Text::String*>> Clone() const;
		virtual OSInt Compare(Text::String *obj1, Text::String *obj2) const;
		OSInt SortedIndexOf(Text::CStringNN str) const;
	};
}
#endif
