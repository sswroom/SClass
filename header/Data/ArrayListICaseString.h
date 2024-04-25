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

		virtual NN<Data::ArrayList<Text::String*>> Clone() const;
		virtual OSInt Compare(Text::String* obj1, Text::String* obj2) const;
		virtual OSInt SortedIndexOfPtr(const UTF8Char *val, UOSInt len) const;
	};
}
#endif
