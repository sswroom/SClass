#ifndef _SM_DATA_ARRAYLISTDBL
#define _SM_DATA_ARRAYLISTDBL
#include "Data/SortableArrayListNative.hpp"

namespace Data
{
	class ArrayListDbl : public Data::SortableArrayListNative<Double>
	{
	public:
		ArrayListDbl();
		ArrayListDbl(NN<Data::ArrayListDbl> src, UIntOS startIndex, UIntOS count);
		ArrayListDbl(UIntOS capacity);

		virtual NN<ArrayListNative<Double>> Clone() const;
		Double FrobeniusNorm() const;
		Double Average() const;
		Double StdDev() const;
		UIntOS Subset(NN<ArrayListDbl> outList, UIntOS firstIndex, UIntOS endIndex) const;
		UIntOS Subset(NN<ArrayListDbl> outList, UIntOS firstIndex) const;
		template<typename T> void AddAllConv(NN<ArrayListNative<T>> src);
		NN<ArrayListDbl> CalcLn();
		NN<ArrayListDbl> CalcAdd(Double val);
	};

	template<typename T> void ArrayListDbl::AddAllConv(NN<ArrayListNative<T>> src)
	{
		this->EnsureCapacity(this->objCnt + src->GetCount());
		UIntOS i = 0;
		UIntOS j = src->GetCount();
		while (i < j)
		{
			this->Add((Double)src->GetItem(i));
			i++;
		}
	}
}
#endif
