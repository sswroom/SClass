#include "Stdafx.h"

extern "C" void Math_Int32Arr2DblArr(Double *dblArr, const Int32 *intArr, UIntOS arrCnt)
{
	UIntOS i = 0;
	while (i < arrCnt)
	{
		dblArr[i] = (Double)intArr[i];
		i++;
	}
}
