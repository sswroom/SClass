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

		virtual NN<ArrayList<Double>> Clone() const;
		Double FrobeniusNorm() const;
		Double Average() const;
		Double StdDev() const;
		UOSInt Subset(NN<ArrayListDbl> outList, UOSInt firstIndex, UOSInt endIndex) const;
		UOSInt Subset(NN<ArrayListDbl> outList, UOSInt firstIndex) const;
	};
}
#endif
