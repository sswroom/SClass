#ifndef _SM_DATA_ARRAYLISTCSTR
#define _SM_DATA_ARRAYLISTCSTR
#include "Data/SortableArrayList.h"
#include "Text/CString.h"

namespace Data
{
	class ArrayListCStr : public Data::SortableArrayList<Text::CString>
	{
	public:
		ArrayListCStr();
		ArrayListCStr(UOSInt capacity);

		virtual Data::ArrayList<Text::CString> *Clone();
		virtual OSInt CompareItem(Text::CString obj1, Text::CString obj2);
	};
}
#endif