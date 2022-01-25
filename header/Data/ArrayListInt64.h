#ifndef _SM_DATA_ARRAYLISTINT64
#define _SM_DATA_ARRAYLISTINT64
#include "Data/SortableArrayListNative.h"

namespace Data
{
	class ArrayListInt64 : public Data::SortableArrayListNative<Int64>
	{
	public:
		ArrayListInt64();
		ArrayListInt64(UOSInt capacity);

		virtual ArrayList<Int64> *Clone();

		void AddRangeI32(Int32 *arr, UOSInt cnt);
		void AddRangeI32(Data::ArrayList<Int32> *arr);
	};
}
#endif
