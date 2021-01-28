#include "Stdafx.h"

extern "C" void Math_Int32Arr2DblArr(Double *dblArr, Int32 *intArr, UOSInt arrCnt)
{
	UOSInt i = 0;
	while (i < arrCnt)
	{
		dblArr[i] = (Double)intArr[i];
		i++;
	}
}
