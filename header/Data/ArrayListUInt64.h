#ifndef _SM_DATA_ARRAYLISTUINT64
#define _SM_DATA_ARRAYLISTUINT64
#include "Data/SortableArrayListNative.h"

namespace Data
{
	class ArrayListUInt64 : public Data::SortableArrayListNative<UInt64>
	{
	public:
		ArrayListUInt64();
		ArrayListUInt64(UOSInt capacity);

		virtual ArrayList<UInt64> *Clone();

		void AddRangeU32(UInt32 *arr, UOSInt cnt);
		void AddRangeU32(Data::ArrayList<UInt32> *arr);
	};
}
#endif
