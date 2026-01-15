#ifndef _SM_DATA_ARRAYLISTTS
#define _SM_DATA_ARRAYLISTTS
#include "Data/SortableArrayListObj.hpp"
#include "Data/Timestamp.h"

namespace Data
{
	class ArrayListTS : public Data::SortableArrayListObj<Data::Timestamp>
	{
	public:
		ArrayListTS();
		ArrayListTS(UIntOS capacity);

		virtual NN<ArrayListObj<Data::Timestamp>> Clone() const;
		virtual IntOS Compare(Data::Timestamp a, Data::Timestamp b) const;
	};
}
#endif
