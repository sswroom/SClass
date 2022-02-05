#ifndef _SM_DATA_ARRAYLISTICASESTRING
#define _SM_DATA_ARRAYLISTICASESTRING
#include "Data/ArrayListString.h"

namespace Data
{
	class ArrayListICaseString : public Data::ArrayListString
	{
	public:
		ArrayListICaseString();
		ArrayListICaseString(UOSInt capacity);

		virtual Data::ArrayList<Text::String*> *Clone();
		virtual OSInt CompareItem(Text::String* obj1, Text::String* obj2);
		virtual OSInt SortedIndexOfPtr(const UTF8Char *val, UOSInt len);
	};
}
#endif
