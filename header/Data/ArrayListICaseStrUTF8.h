#ifndef _SM_DATA_ARRAYLISTICASESTRUTF8
#define _SM_DATA_ARRAYLISTICASESTRUTF8
#include "Data/ArrayListStrUTF8.h"

namespace Data
{
	class ArrayListICaseStrUTF8 : public Data::ArrayListStrUTF8
	{
	public:
		ArrayListICaseStrUTF8();
		ArrayListICaseStrUTF8(OSInt Capacity);

		virtual Data::ArrayList<const UTF8Char*> *Clone();

		virtual UOSInt SortedInsert(const UTF8Char *val);
		virtual OSInt SortedIndexOf(const UTF8Char *val);
	};
}
#endif
