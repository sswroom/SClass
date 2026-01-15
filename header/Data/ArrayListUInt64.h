#ifndef _SM_DATA_ARRAYLISTUINT64
#define _SM_DATA_ARRAYLISTUINT64
#include "Data/SortableArrayListNative.hpp"

namespace Data
{
	class ArrayListUInt64 : public Data::SortableArrayListNative<UInt64>
	{
	public:
		ArrayListUInt64();
		ArrayListUInt64(UIntOS capacity);

		virtual NN<ArrayListNative<UInt64>> Clone() const;

		void AddRangeU32(UnsafeArray<const UInt32> arr, UIntOS cnt);
		void AddRangeU32(NN<const Data::ArrayListNative<UInt32>> arr);
	};
}
#endif
