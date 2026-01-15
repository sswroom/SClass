#ifndef _SM_DATA_ARRAYLISTINT32
#define _SM_DATA_ARRAYLISTINT32
#include "Data/SortableArrayListNative.hpp"

namespace Data
{
	class ArrayListInt32 : public Data::SortableArrayListNative<Int32>
	{
	public:
		ArrayListInt32();
		ArrayListInt32(UIntOS capacity);

		virtual NN<ArrayListNative<Int32>> Clone() const;
	};
}
#endif
