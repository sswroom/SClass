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

		virtual Data::ArrayList<Text::String*> *Clone() const;
		virtual OSInt Compare(Text::String* obj1, Text::String* obj2) const;
		virtual OSInt SortedIndexOfPtr(const UTF8Char *val, UOSInt len) const;

		Text::String *JoinString() const;
	};
}
#endif
