#ifndef _SM_DATA_ARRAYLISTSTRUTF8
#define _SM_DATA_ARRAYLISTSTRUTF8
#include "Data/SortableArrayList.h"

namespace Data
{
	class ArrayListStrUTF8 : public Data::SortableArrayList<const UTF8Char*>
	{
	public:
		ArrayListStrUTF8();
		ArrayListStrUTF8(OSInt Capacity);

		virtual Data::ArrayList<const UTF8Char*> *Clone();

		virtual UOSInt SortedInsert(const UTF8Char *val);
		virtual OSInt SortedIndexOf(const UTF8Char *val);
		const UTF8Char *JoinNewStr();
	};
}
#endif
