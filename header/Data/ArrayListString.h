#ifndef _SM_DATA_ARRAYLISTSTRING
#define _SM_DATA_ARRAYLISTSTRING
#include "Data/SortableArrayList.h"
#include "Text/String.h"

namespace Data
{
	class ArrayListString : public Data::SortableArrayList<Text::String*>
	{
	public:
		ArrayListString();
		ArrayListString(UOSInt capacity);

		virtual Data::ArrayList<Text::String*> *Clone();
		virtual OSInt CompareItem(Text::String* obj1, Text::String* obj2);
		virtual OSInt SortedIndexOfPtr(const UTF8Char *val, UOSInt len);

		Text::String *JoinString();
	};
}
#endif
