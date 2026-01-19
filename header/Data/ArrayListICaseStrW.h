#ifndef _SM_DATA_ARRAYLISTICASESTRW
#define _SM_DATA_ARRAYLISTICASESTRW
#include "Data/ArrayListStrW.h"

namespace Data
{
	class ArrayListICaseStrW : public Data::ArrayListStrW
	{
	public:
		ArrayListICaseStrW();
		ArrayListICaseStrW(UIntOS Capacity);

		virtual NN<Data::ArrayListObj<const WChar*>> Clone() const;
		virtual IntOS Compare(const WChar* obj1, const WChar* obj2) const;
	};
}
#endif
