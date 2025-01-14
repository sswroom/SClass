#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/IComparable.h"
#include "Text/MyString.h"

extern "C" void BubbleSort_SortInt32(Int32 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt j = lastIndex;
	OSInt i;
	Int32 *b;
	Bool finished = false;
	Int32 tmp;
	Int32 *c;
	while (j-- > firstIndex)
	{
		finished = true;
		b = &arr[firstIndex];
		i = j - firstIndex + 1;
		while (i--)
		{
			c = b++;
			if (*c > *b)
			{
				tmp = *b;
				*b = *c;
				*c = tmp;
				finished = false;
			}
		}
		if (finished)
			break;
	}
}

extern "C" void BubbleSort_SortUInt32(UInt32 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt j = lastIndex - firstIndex;
	OSInt i;
	UInt32 *b;
	Bool finished = false;
	UInt32 v1;
	UInt32 v2;
	arr += firstIndex;
	while (j-- > 0)
	{
		finished = true;
		b = arr;
		i = j + 1;
		v1 = *b++;
		while (i--)
		{
			v2 = *b++;
			if (v1 > v2)
			{
				b[-2] = v2;
				b[-1] = v1;
				finished = false;
			}
			else
			{
				v1 = v2;
			}
		}
		if (finished)
			break;
	}
}

extern "C" void BubbleSort_SortInt64(Int64 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt j = lastIndex - firstIndex;
	OSInt i;
	Int64 *b;
	Bool finished = false;
	Int64 v1;
	Int64 v2;
	arr += firstIndex;
	while (j-- > 0)
	{
		finished = true;
		b = arr;
		i = j + 1;
		v1 = *b++;
		while (i--)
		{
			v2 = *b++;
			if (v1 > v2)
			{
				b[-2] = v2;
				b[-1] = v1;
				finished = false;
			}
			else
			{
				v1 = v2;
			}
		}
		if (finished)
			break;
	}
}

extern "C" void BubbleSort_SortDouble(Double *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt j = lastIndex - firstIndex;
	OSInt i;
	Double *b;
	Bool finished = false;
	Double v1;
	Double v2;
	arr += firstIndex;
	while (j-- > 0)
	{
		finished = true;
		b = arr;
		i = j + 1;
		v1 = *b++;
		while (i--)
		{
			v2 = *b++;
			if (v1 > v2)
			{
				b[-2] = v2;
				b[-1] = v1;
				finished = false;
			}
			else
			{
				v1 = v2;
			}
		}
		if (finished)
			break;
	}
}

extern "C" void BubbleSort_SortSingle(Single *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt j = lastIndex - firstIndex;
	OSInt i;
	Single *b;
	Bool finished = false;
	Single v1;
	Single v2;
	arr += firstIndex;
	while (j-- > 0)
	{
		finished = true;
		b = arr;
		i = j + 1;
		v1 = *b++;
		while (i--)
		{
			v2 = *b++;
			if (v1 > v2)
			{
				b[-2] = v2;
				b[-1] = v1;
				finished = false;
			}
			else
			{
				v1 = v2;
			}
		}
		if (finished)
			break;
	}
}

extern "C" void BubbleSort_SortStrC(Char **arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt j = lastIndex - firstIndex;
	OSInt i;
	Char **b;
	Bool finished = false;
	Char *v1;
	Char *v2;
	arr += firstIndex;
	while (j-- > 0)
	{
		finished = true;
		b = arr;
		i = j + 1;
		v1 = *b++;
		while (i--)
		{
			v2 = *b++;
			if (Text::StrCompareCh(v1, v2) > 0)
			{
				b[-2] = v2;
				b[-1] = v1;
				finished = false;
			}
			else
			{
				v1 = v2;
			}
		}
		if (finished)
			break;
	}
}

/*extern "C" void BubbleSort_SortCmp(void **arr, Data::IComparable::CompareFunc func, OSInt firstIndex, OSInt lastIndex)
{
	OSInt j = lastIndex - firstIndex;
	OSInt i;
	void **b;
	Bool finished = false;
	void *v1;
	void *v2;
	arr += firstIndex;
	while (j-- > 0)
	{
		finished = true;
		b = arr;
		i = j + 1;
		v1 = *b++;
		while (i--)
		{
			v2 = *b++;
			if (func(v1, v2) > 0)
			{
				b[-2] = v2;
				b[-1] = v1;
				finished = false;
			}
			else
			{
				v1 = v2;
			}
		}
		if (finished)
			break;
	}
}*/

extern "C" void BubbleSort_SortInt64Inv(Int64 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt j = lastIndex - firstIndex;
	OSInt i;
	Int64 *b;
	Bool finished = false;
	Int64 v1;
	Int64 v2;
	arr += firstIndex;
	while (j-- > 0)
	{
		finished = true;
		b = arr;
		i = j + 1;
		v1 = *b++;
		while (i--)
		{
			v2 = *b++;
			if (v1 < v2)
			{
				b[-2] = v2;
				b[-1] = v1;
				finished = false;
			}
			else
			{
				v1 = v2;
			}
		}
		if (finished)
			break;
	}
}

extern "C" void BubbleSort_SortDoubleInv(Double *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt j = lastIndex - firstIndex;
	OSInt i;
	Double *b;
	Bool finished = false;
	Double v1;
	Double v2;
	arr += firstIndex;
	while (j-- > 0)
	{
		finished = true;
		b = arr;
		i = j + 1;
		v1 = *b++;
		while (i--)
		{
			v2 = *b++;
			if (v1 < v2)
			{
				b[-2] = v2;
				b[-1] = v1;
				finished = false;
			}
			else
			{
				v1 = v2;
			}
		}
		if (finished)
			break;
	}
}

extern "C" void BubbleSort_SortSingleInv(Single *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt j = lastIndex - firstIndex;
	OSInt i;
	Single *b;
	Bool finished = false;
	Single v1;
	Single v2;
	arr += firstIndex;
	while (j-- > 0)
	{
		finished = true;
		b = arr;
		i = j + 1;
		v1 = *b++;
		while (i--)
		{
			v2 = *b++;
			if (v1 < v2)
			{
				b[-2] = v2;
				b[-1] = v1;
				finished = false;
			}
			else
			{
				v1 = v2;
			}
		}
		if (finished)
			break;
	}
}

extern "C" void BubbleSort_SortStrCInv(Char **arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt j = lastIndex - firstIndex;
	OSInt i;
	Char **b;
	Bool finished = false;
	Char *v1;
	Char *v2;
	arr += firstIndex;
	while (j-- > 0)
	{
		finished = true;
		b = arr;
		i = j + 1;
		v1 = *b++;
		while (i--)
		{
			v2 = *b++;
			if (Text::StrCompareCh(v1, v2) < 0)
			{
				b[-2] = v2;
				b[-1] = v1;
				finished = false;
			}
			else
			{
				v1 = v2;
			}
		}
		if (finished)
			break;
	}
}

/*extern "C" void BubbleSort_SortCmpInv(void **arr, Data::IComparable::CompareFunc func, OSInt firstIndex, OSInt lastIndex)
{
	OSInt j = lastIndex - firstIndex;
	OSInt i;
	void **b;
	Bool finished = false;
	void *v1;
	void *v2;
	arr += firstIndex;
	while (j-- > 0)
	{
		finished = true;
		b = arr;
		i = j + 1;
		v1 = *b++;
		while (i--)
		{
			v2 = *b++;
			if (func(v1, v2) < 0)
			{
				b[-2] = v2;
				b[-1] = v1;
				finished = false;
			}
			else
			{
				v1 = v2;
			}
		}
		if (finished)
			break;
	}
}*/

