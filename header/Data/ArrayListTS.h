#ifndef _SM_DATA_ARRAYLISTTS
#define _SM_DATA_ARRAYLISTTS
#include "Data/SortableArrayListNative.hpp"

namespace Data
{
	class ArrayListTS : public Data::SortableArrayListNative<Data::Timestamp>
	{
	public:
		ArrayListTS();
		ArrayListTS(UIntOS capacity);

		virtual NN<ArrayListNative<Data::Timestamp>> Clone() const;
	};
}
#endif
