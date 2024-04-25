#ifndef _SM_DATA_ARRAYLISTCSTRFAST
#define _SM_DATA_ARRAYLISTCSTRFAST
#include "Data/SortableArrayList.h"
#include "Text/CString.h"

namespace Data
{
	class ArrayListCStrFast : public Data::SortableArrayList<Text::CString>
	{
	public:
		ArrayListCStrFast();
		ArrayListCStrFast(UOSInt capacity);

		virtual NN<Data::ArrayList<Text::CString>> Clone() const;
		virtual OSInt Compare(Text::CString obj1, Text::CString obj2) const;
	};
}
#endif
