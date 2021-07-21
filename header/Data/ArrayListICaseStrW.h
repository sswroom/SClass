#ifndef _SM_DATA_ARRAYLISTICASESTRW
#define _SM_DATA_ARRAYLISTICASESTRW
#include "Data/ArrayListStrW.h"

namespace Data
{
	class ArrayListICaseStrW : public Data::ArrayListStrW
	{
	public:
		ArrayListICaseStrW();
		ArrayListICaseStrW(OSInt Capacity);

		virtual Data::ArrayList<const WChar*> *Clone();
		virtual OSInt CompareItem(const WChar* obj1, const WChar* obj2);
	};
}
#endif
