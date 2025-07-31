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
