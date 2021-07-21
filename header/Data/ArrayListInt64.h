#ifndef _SM_DATA_ARRAYLISTINT64
#define _SM_DATA_ARRAYLISTINT64
#include "Data/SortableArrayList.h"

namespace Data
{
	class ArrayListInt64 : public Data::SortableArrayList<Int64>
	{
	public:
		ArrayListInt64();
		ArrayListInt64(UOSInt capacity);

		virtual ArrayList<Int64> *Clone();
		virtual OSInt CompareItem(Int64 obj1, Int64 obj2);

		void AddRangeI32(Int32 *arr, UOSInt cnt);
		void AddRangeI32(Data::ArrayList<Int32> *arr);
	};
}
#endif
