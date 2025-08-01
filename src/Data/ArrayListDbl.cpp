#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListDbl.h"
#include "Data/Comparable.h"

Data::ArrayListDbl::ArrayListDbl() : Data::SortableArrayListNative<Double>()
{
}

Data::ArrayListDbl::ArrayListDbl(UOSInt capacity) : Data::SortableArrayListNative<Double>(capacity)
{
}

NN<Data::ArrayList<Double>> Data::ArrayListDbl::Clone() const
{
	NN<Data::ArrayList<Double>> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListDbl(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

Double Data::ArrayListDbl::FrobeniusNorm() const
{
	Double sum = 0;
	UOSInt i = this->objCnt;
	while (i-- > 0)
	{
		sum += this->arr[i] * this->arr[i];
	}
	return Math_Sqrt(sum);
}

Double Data::ArrayListDbl::Average() const
{
	Double sum = 0;
	UOSInt i = this->objCnt;
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
	UOSInt i = this->objCnt;
	while (i-- > 0)
	{
		d = this->arr[i] - avg;
		sum += d * d;
	}
	return Math_Sqrt(sum / (Double)this->objCnt);
}
