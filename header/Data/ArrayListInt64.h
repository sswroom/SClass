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

		virtual NN<ArrayList<Int64>> Clone() const;

		void AddRangeI32(Int32 *arr, UOSInt cnt);
		void AddRangeI32(const Data::ArrayList<Int32> *arr);
	};
}
#endif
