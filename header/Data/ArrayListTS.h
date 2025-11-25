#ifndef _SM_DATA_ARRAYLISTTS
#define _SM_DATA_ARRAYLISTTS
#include "Data/SortableArrayListNative.hpp"

namespace Data
{
	class ArrayListTS : public Data::SortableArrayListNative<Data::Timestamp>
	{
	public:
		ArrayListTS();
		ArrayListTS(UOSInt capacity);

		virtual NN<ArrayList<Data::Timestamp>> Clone() const;
	};
}
#endif
