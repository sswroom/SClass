#ifndef _SM_DATA_ARRAYLISTSNG
#define _SM_DATA_ARRAYLISTSNG
#include "Data/SortableArrayListNative.h"

namespace Data
{
	class ArrayListSng : public Data::SortableArrayListNative<Single>
	{
	public:
		ArrayListSng();
		ArrayListSng(OSInt Capacity);

		virtual NN<Data::ArrayList<Single>> Clone() const;
	};
}
#endif
