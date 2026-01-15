#ifndef _SM_DATA_ARRAYLISTDBL
#define _SM_DATA_ARRAYLISTDBL
#include "Data/SortableArrayListNative.hpp"

namespace Data
{
	class ArrayListDbl : public Data::SortableArrayListNative<Double>
	{
	public:
		ArrayListDbl();
		ArrayListDbl(UIntOS capacity);

		virtual NN<ArrayListNative<Double>> Clone() const;
		Double FrobeniusNorm() const;
		Double Average() const;
		Double StdDev() const;
		UIntOS Subset(NN<ArrayListDbl> outList, UIntOS firstIndex, UIntOS endIndex) const;
		UIntOS Subset(NN<ArrayListDbl> outList, UIntOS firstIndex) const;
	};
}
#endif
