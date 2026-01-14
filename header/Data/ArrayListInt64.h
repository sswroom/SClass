#ifndef _SM_DATA_ARRAYLISTINT64
#define _SM_DATA_ARRAYLISTINT64
#include "Data/SortableArrayListNative.hpp"

namespace Data
{
	class ArrayListInt64 : public Data::SortableArrayListNative<Int64>
	{
	public:
		ArrayListInt64();
		ArrayListInt64(UOSInt capacity);

		virtual NN<ArrayListNative<Int64>> Clone() const;

		void AddRangeI32(UnsafeArray<Int32> arr, UOSInt cnt);
		void AddRangeI32(NN<const Data::ArrayListNative<Int32>> arr);
	};
}
#endif
