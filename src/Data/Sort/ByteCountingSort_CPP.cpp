#include "Stdafx.h"
#include "MyMemory.h"

extern "C" Bool ByteCountingSort_SortUInt32(UInt32 *arr, OSInt firstIndex, OSInt lastIndex)
{
	arr += firstIndex;
	OSInt arrLen = lastIndex - firstIndex + 1;
	UInt32 *arr2 = MemAlloc(UInt32, arrLen);
	if (arr2 == 0)
		return false;
	UInt32 *arrTmp;
	UOSInt dynArr3[4][256] = {{0}};
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt m;
	i = 0;
	while (i < arrLen)
	{
		m = arr[i] & 0xff;
		dynArr3[0][m] = dynArr3[0][m] + 1;
		m = (arr[i] >> 8) & 0xff;
		dynArr3[1][m] = dynArr3[1][m] + 1;
		m = (arr[i] >> 16) & 0xff;
		dynArr3[2][m] = dynArr3[2][m] + 1;
		m = (arr[i] >> 24) & 0xff;
		dynArr3[3][m] = dynArr3[3][m] + 1;

		i++;
	}

	k = 0;
	while (k < 4)
	{
		m = 0;
		j = 0;
		while (j < 256)
		{
			i = dynArr3[k][j];
			if (i != 0)
			{
				dynArr3[k][j] = m;
				m = m + i;
			}
			j++;
		}

		j = 0;
		while (j < arrLen)
		{
			m = arr[j] >> (8 * k) & 0xFF;
			arr2[dynArr3[k][m]] = arr[j];
			dynArr3[k][m] = dynArr3[k][m] + 1;
			j++;
		}

		arrTmp = arr;
		arr = arr2;
		arr2 = arrTmp;

		k++;
	}
	MemFree(arr2);
	return true;
}
