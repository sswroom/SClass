#ifndef _SM_DATA_ARRAYLISTSTRUTF8
#define _SM_DATA_ARRAYLISTSTRUTF8
#include "Data/SortableArrayList.h"

namespace Data
{
	class ArrayListStrUTF8 : public Data::SortableArrayList<const UTF8Char*>
	{
	public:
		ArrayListStrUTF8();
		ArrayListStrUTF8(UOSInt capacity);

		virtual Data::ArrayList<const UTF8Char*> *Clone();
		virtual OSInt CompareItem(const UTF8Char* obj1, const UTF8Char* obj2);
		const UTF8Char *JoinNewStr();
	};
}
#endif
