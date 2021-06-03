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

		virtual UOSInt SortedInsert(Text::String *val);
		virtual OSInt SortedIndexOf(Text::String *val);
		Text::String *JoinString();
	};
}
#endif
