#ifndef _SM_DATA_ARRAYLISTSTRFAST
#define _SM_DATA_ARRAYLISTSTRFAST
#include "Data/SortableArrayList.h"
#include "Text/String.h"

namespace Data
{
	class ArrayListStrFast : public Data::SortableArrayList<Text::String*>
	{
	public:
		ArrayListStrFast();
		ArrayListStrFast(UOSInt capacity);

		virtual Data::ArrayList<Text::String*> *Clone() const;
		virtual OSInt CompareItem(Text::String *obj1, Text::String *obj2) const;
		OSInt SortedIndexOf(Text::CString str) const;
	};
}
#endif