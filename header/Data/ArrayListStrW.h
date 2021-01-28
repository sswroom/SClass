#ifndef _SM_DATA_ARRAYLISTSTRW
#define _SM_DATA_ARRAYLISTSTRW
#include "Data/SortableArrayList.h"

namespace Data
{
	class ArrayListStrW : public Data::SortableArrayList<const WChar*>
	{
	public:
		ArrayListStrW();
		ArrayListStrW(OSInt Capacity);

		virtual Data::ArrayList<const WChar*> *Clone();

		virtual UOSInt SortedInsert(const WChar *val);
		virtual OSInt SortedIndexOf(const WChar *val);
		const WChar *JoinNewStr();
	};
}
#endif
