#ifndef _SM_DATA_ARRAYLISTICASESTRW
#define _SM_DATA_ARRAYLISTICASESTRW
#include "Data/ArrayListStrW.h"

namespace Data
{
	class ArrayListICaseStrW : public Data::ArrayListStrW
	{
	public:
		ArrayListICaseStrW();
		ArrayListICaseStrW(UOSInt Capacity);

		virtual NN<Data::ArrayList<const WChar*>> Clone() const;
		virtual OSInt Compare(const WChar* obj1, const WChar* obj2) const;
	};
}
#endif
