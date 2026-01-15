#ifndef _SM_DATA_ARRAYLISTUINT32
#define _SM_DATA_ARRAYLISTUINT32
#include "Data/SortableArrayListNative.hpp"

namespace Data
{
	class ArrayListUInt32 : public Data::SortableArrayListNative<UInt32>
	{
	public:
		ArrayListUInt32();
		ArrayListUInt32(UIntOS capacity);
		virtual ~ArrayListUInt32();

		virtual NN<ArrayListNative<UInt32>> Clone() const;
	};
}
#endif
