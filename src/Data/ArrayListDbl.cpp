#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListDbl.h"
#include "Data/Comparable.h"

Data::ArrayListDbl::ArrayListDbl() : Data::SortableArrayListNative<Double>()
{
}

Data::ArrayListDbl::ArrayListDbl(UIntOS capacity) : Data::SortableArrayListNative<Double>(capacity)
{
}

NN<Data::ArrayListNative<Double>> Data::ArrayListDbl::Clone() const
{
	NN<Data::ArrayListNative<Double>> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListDbl(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

Double Data::ArrayListDbl::FrobeniusNorm() const
{
	Double sum = 0;
	UIntOS i = this->objCnt;
	while (i-- > 0)
	{
		sum += this->arr[i] * this->arr[i];
	}
	return Math_Sqrt(sum);
}

Double Data::ArrayListDbl::Average() const
{
	Double sum = 0;
	UIntOS i = this->objCnt;
	while (i-- > 0)
	{
		sum += this->arr[i];
	}
	return sum / (Double)this->objCnt;
}

Double Data::ArrayListDbl::StdDev() const
{
	Double avg = this->Average();
	Double sum = 0;
	Double d;
	UIntOS i = this->objCnt;
	while (i-- > 0)
	{
		d = this->arr[i] - avg;
		sum += d * d;
	}
	return Math_Sqrt(sum / (Double)this->objCnt);
}

UIntOS Data::ArrayListDbl::Subset(NN<ArrayListDbl> outList, UIntOS firstIndex, UIntOS endIndex) const
{
	if (firstIndex > this->objCnt)
		firstIndex = this->objCnt;
	if (endIndex > this->objCnt)
		endIndex = this->objCnt;
	if (firstIndex < endIndex)
	{
		outList->AddRange(&this->arr[firstIndex], endIndex - firstIndex);
		return endIndex - firstIndex;
	}
	return 0;
}

UIntOS Data::ArrayListDbl::Subset(NN<ArrayListDbl> outList, UIntOS firstIndex) const
{
	return Subset(outList, firstIndex, this->objCnt);
}
