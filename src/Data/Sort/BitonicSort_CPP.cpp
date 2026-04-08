#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"

void BitonicSort_MergeInt32(Int32 *arr, IntOS n, Bool dir, IntOS m)
{
	Int32 v1;
	Int32 v2;
	IntOS i = 0;
	while (i < n - m)
	{
		v1 = arr[i];
		v2 = arr[i + m];
		if (dir == (v1 > v2))
		{
			arr[i] = v2;
			arr[i + m] = v1;
		}
		i++;
	}
	if (m > 1)
	{
		BitonicSort_MergeInt32(arr, m, dir, m >> 1);

		n -= m;
		if (n > 1)
		{
			arr = arr + m;
			while (m >= n)
			{
				m = m >> 1;
			}
			BitonicSort_MergeInt32(arr, n, dir, m);
		}
	}
	else if (n > 2)
	{
		v1 = arr[1];
		v2 = arr[2];
		if (dir == (v1 > v2))
		{
			arr[1] = v2;
			arr[2] = v1;
		}
	}
}

void BitonicSort_MergeUInt32(UInt32 *arr, IntOS n, Bool dir, IntOS m)
{
	UInt32 v1;
	UInt32 v2;
	IntOS i = 0;
	while (i < n - m)
	{
		v1 = arr[i];
		v2 = arr[i + m];
		if (dir == (v1 > v2))
		{
			arr[i] = v2;
			arr[i + m] = v1;
		}
		i++;
	}
	if (m > 1)
	{
		BitonicSort_MergeUInt32(arr, m, dir, m >> 1);

		n -= m;
		if (n > 1)
		{
			arr = arr + m;
			while (m >= n)
			{
				m = m >> 1;
			}
			BitonicSort_MergeUInt32(arr, n, dir, m);
		}
	}
	else if (n > 2)
	{
		v1 = arr[1];
		v2 = arr[2];
		if (dir == (v1 > v2))
		{
			arr[1] = v2;
			arr[2] = v1;
		}
	}
}

void BitonicSort_MergeStr(UTF8Char **arr, IntOS n, Bool dir, IntOS m)
{
	UTF8Char *v1;
	UTF8Char *v2;
	IntOS i = 0;
	while (i < n - m)
	{
		v1 = arr[i];
		v2 = arr[i + m];
		if (dir == (Text::StrCompare(v1, v2) > 0))
		{
			arr[i] = v2;
			arr[i + m] = v1;
		}
		i++;
	}
	if (m > 1)
	{
		BitonicSort_MergeStr(arr, m, dir, m >> 1);

		n -= m;
		if (n > 1)
		{
			arr = arr + m;
			while (m >= n)
			{
				m = m >> 1;
			}
			BitonicSort_MergeStr(arr, n, dir, m);
		}
	}
	else if (n > 2)
	{
		v1 = arr[1];
		v2 = arr[2];
		if (dir == (Text::StrCompare(v1, v2) > 0))
		{
			arr[1] = v2;
			arr[2] = v1;
		}
	}
}

void BitonicSort_SortInnerInt32(Int32 *arr, IntOS n, Bool dir, IntOS pw2)
{
	IntOS m = n / 2;
	if (m > 1)
	{
		BitonicSort_SortInnerInt32(arr, m, !dir, pw2 >> 1);
		BitonicSort_SortInnerInt32(arr + m, n - m, dir, pw2 >> 1);
	}
	else if (n > 2)
	{
		Int32 v1 = arr[1];
		Int32 v2 = arr[2];
		if (dir == (v1 > v2))
		{
			arr[1] = v2;
			arr[2] = v1;
		}
	}
	BitonicSort_MergeInt32(arr, n, dir, pw2);
}

void BitonicSort_SortInnerUInt32(UInt32 *arr, IntOS n, Bool dir, IntOS pw2)
{
	IntOS m = n / 2;
	if (m > 1)
	{
		BitonicSort_SortInnerUInt32(arr, m, !dir, pw2 >> 1);
		BitonicSort_SortInnerUInt32(arr + m, n - m, dir, pw2 >> 1);
	}
	else if (n > 2)
	{
		UInt32 v1 = arr[1];
		UInt32 v2 = arr[2];
		if (dir == (v1 > v2))
		{
			arr[1] = v2;
			arr[2] = v1;
		}
	}
	BitonicSort_MergeUInt32(arr, n, dir, pw2);
}

void BitonicSort_SortInnerStr(UTF8Char **arr, IntOS n, Bool dir, IntOS pw2)
{
	IntOS m = n / 2;
	if (m > 1)
	{
		BitonicSort_SortInnerStr(arr, m, !dir, pw2 >> 1);
		BitonicSort_SortInnerStr(arr + m, n - m, dir, pw2 >> 1);
	}
	else if (n > 2)
	{
		UTF8Char *v1 = arr[1];
		UTF8Char *v2 = arr[2];
		if (dir == (Text::StrCompare(v1, v2) > 0))
		{
			arr[1] = v2;
			arr[2] = v1;
		}
	}
	BitonicSort_MergeStr(arr, n, dir, pw2);
}

extern "C" void BitonicSort_SortInt32(Int32 *arr, IntOS firstIndex, IntOS lastIndex)
{
	IntOS cnt = lastIndex - firstIndex + 1;
	if (cnt > 1)
	{
		IntOS m = 1;
		while (m <= cnt)
			m = m << 1;
		BitonicSort_SortInnerInt32(arr + firstIndex, cnt, true, m >> 1);
	}
}

extern "C" void BitonicSort_SortUInt32(UInt32 *arr, IntOS firstIndex, IntOS lastIndex)
{
	IntOS cnt = lastIndex - firstIndex + 1;
	if (cnt > 1)
	{
		IntOS m = 1;
		while (m <= cnt)
			m = m << 1;
		BitonicSort_SortInnerUInt32(arr + firstIndex, cnt, true, m >> 1);
	}
}

extern "C" void BitonicSort_SortStr(UTF8Char **arr, IntOS firstIndex, IntOS lastIndex)
{
	IntOS cnt = lastIndex - firstIndex + 1;
	if (cnt > 1)
	{
		IntOS m = 1;
		while (m <= cnt)
			m = m << 1;
		BitonicSort_SortInnerStr(arr + firstIndex, cnt, true, m >> 1);
	}
}
