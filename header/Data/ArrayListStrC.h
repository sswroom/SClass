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

		virtual NN<Data::ArrayList<const Char*>> Clone() const;
		virtual OSInt Compare(const Char* obj1, const Char* obj2) const;
	};
}
#endif
