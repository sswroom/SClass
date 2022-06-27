#ifndef _SM_DATA_ARRAYLISTSTRC
#define _SM_DATA_ARRAYLISTSTRC
#include "Data/SortableArrayList.h"

namespace Data
{
	class ArrayListStrC : public Data::SortableArrayList<const Char*>
	{
	public:
		ArrayListStrC();
		ArrayListStrC(UOSInt capacity);

		virtual Data::ArrayList<const Char*> *Clone() const;
		virtual OSInt CompareItem(const Char* obj1, const Char* obj2) const;
	};
}
#endif
