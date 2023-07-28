#ifndef _SM_DATA_ARRAYLISTDBL
#define _SM_DATA_ARRAYLISTDBL
#include "Data/SortableArrayListNative.h"

namespace Data
{
	class ArrayListDbl : public Data::SortableArrayListNative<Double>
	{
	public:
		ArrayListDbl();
		ArrayListDbl(UOSInt capacity);

		virtual NotNullPtr<ArrayList<Double>> Clone() const;
	};
}
#endif
