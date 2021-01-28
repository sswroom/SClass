#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/IComparable.h"
#include "Text/MyString.h"

extern "C" void InsertionSort_SortBInt32(Int32 *arr, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	Int32 temp;
	Int32 temp1;
	Int32 temp2;
	temp1 = arr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr[i];
		if ( temp1 > temp2 )
		{
			j = left;
			k = i - 1;
			while (j <= k)
			{
				l = (j + k) >> 1;
				temp = arr[l];
				if (temp > temp2 )
				{
					k = l - 1;
				}
				else
				{
					j = l + 1;
				}
			}
			MemCopyO(&arr[j + 1], &arr[j], (i - j) * sizeof(arr[0]));
			arr[j] = temp2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}

extern "C" void InsertionSort_SortBStr(UTF8Char **arr, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	UTF8Char *temp;
	UTF8Char *temp1;
	UTF8Char *temp2;
	temp1 = arr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr[i];
		if ( Text::StrCompare(temp1, temp2) > 0)
		{
			j = left;
			k = i - 1;
			while (j <= k)
			{
				l = (j + k) >> 1;
				temp = arr[l];
				if (Text::StrCompare(temp, temp2) > 0)
				{
					k = l - 1;
				}
				else
				{
					j = l + 1;
				}
			}
			MemCopyO(&arr[j + 1], &arr[j], (i - j) * sizeof(arr[0]));
			arr[j] = temp2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}

extern "C" void InsertionSort_SortBCmp(void **arr, Data::IComparable::CompareFunc func, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	void *temp;
	void *temp1;
	void *temp2;
	temp1 = arr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr[i];
		if ( func(temp1, temp2) > 0)
		{
			j = left;
			k = i - 1;
			while (j <= k)
			{
				l = (j + k) >> 1;
				temp = arr[l];
				if (func(temp, temp2) > 0)
				{
					k = l - 1;
				}
				else
				{
					j = l + 1;
				}
			}
			MemCopyO(&arr[j + 1], &arr[j], (i - j) * sizeof(arr[0]));
			arr[j] = temp2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}

extern "C" void InsertionSort_SortBCmpO(Data::IComparable **arr, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	Data::IComparable *temp;
	Data::IComparable *temp1;
	Data::IComparable *temp2;
	temp1 = arr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr[i];
		if ( temp1->CompareTo(temp2) > 0)
		{
			j = left;
			k = i - 1;
			while (j <= k)
			{
				l = (j + k) >> 1;
				temp = arr[l];
				if (temp->CompareTo(temp2) > 0)
				{
					k = l - 1;
				}
				else
				{
					j = l + 1;
				}
			}
			MemCopyO(&arr[j + 1], &arr[j], (i - j) * sizeof(arr[0]));
			arr[j] = temp2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}

extern "C" void InsertionSort_SortInt32(Int32 *arr, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	Int32 temp;
	Int32 temp1;
	Int32 temp2;
	temp1 = arr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr[i];
		if ( temp1 > temp2 )
		{
			arr[i] = temp1;
			j = i - 1;
			while (j > left)
			{
				temp = arr[j-1];
				if ( temp > temp2 )
					arr[j--] = temp;
				else
					break;
			}
			arr[j] = temp2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}

extern "C" void InsertionSort_SortUInt32(UInt32 *arr, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	UInt32 temp;
	UInt32 temp1;
	UInt32 temp2;
	temp1 = arr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr[i];
		if ( temp1 > temp2 )
		{
			arr[i] = temp1;
			j = i - 1;
			while (j > left)
			{
				temp = arr[j-1];
				if ( temp > temp2 )
					arr[j--] = temp;
				else
					break;
			}
			arr[j] = temp2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}

extern "C" void InsertionSort_SortInt64(Int64 *arr, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	Int64 temp;
	Int64 temp1;
	Int64 temp2;
	temp1 = arr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr[i];
		if ( temp1 > temp2 )
		{
			arr[i] = temp1;
			j = i - 1;
			while (j > left)
			{
				temp = arr[j-1];
				if ( temp > temp2 )
					arr[j--] = temp;
				else
					break;
			}
			arr[j] = temp2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}

extern "C" void InsertionSort_SortSingle(Single *arr, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	Single temp;
	Single temp1;
	Single temp2;
	temp1 = arr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr[i];
		if ( temp1 > temp2 )
		{
			arr[i] = temp1;
			j = i - 1;
			while (j > left)
			{
				temp = arr[j-1];
				if ( temp > temp2 )
					arr[j--] = temp;
				else
					break;
			}
			arr[j] = temp2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}

extern "C" void InsertionSort_SortDouble(Double *arr, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	Double temp;
	Double temp1;
	Double temp2;
	temp1 = arr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr[i];
		if ( temp1 > temp2 )
		{
			arr[i] = temp1;
			j = i - 1;
			while (j > left)
			{
				temp = arr[j-1];
				if ( temp > temp2 )
					arr[j--] = temp;
				else
					break;
			}
			arr[j] = temp2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}

extern "C" void InsertionSort_SortStr(UTF8Char **arr, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	UTF8Char *temp;
	UTF8Char *temp1;
	UTF8Char *temp2;
	temp1 = arr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr[i];
		if ( Text::StrCompare(temp1, temp2) > 0 )
		{
			arr[i] = temp1;
			j = i - 1;
			while (j > left)
			{
				temp = arr[j-1];
				if ( Text::StrCompare(temp, temp2) > 0 )
					arr[j--] = temp;
				else
					break;
			}
			arr[j] = temp2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}

extern "C" void InsertionSort_SortCmp(void **arr, Data::IComparable::CompareFunc func, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	void *temp;
	void *temp1;
	void *temp2;
	temp1 = arr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr[i];
		if (func(temp1, temp2) > 0)
		{
			arr[i] = temp1;
			j = i - 1;
			while (j > left)
			{
				temp = arr[j-1];
				if (func(temp, temp2) > 0)
					arr[j--] = temp;
				else
					break;
			}
			arr[j] = temp2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}

extern "C" void InsertionSort_SortInt32Inv(Int32 *arr, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	Int32 temp;
	Int32 temp1;
	Int32 temp2;
	temp1 = arr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr[i];
		if ( temp1 < temp2 )
		{
			arr[i] = temp1;
			j = i - 1;
			while (j > left)
			{
				temp = arr[j-1];
				if ( temp < temp2 )
					arr[j--] = temp;
				else
					break;
			}
			arr[j] = temp2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}

extern "C" void InsertionSort_SortUInt32Inv(UInt32 *arr, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	UInt32 temp;
	UInt32 temp1;
	UInt32 temp2;
	temp1 = arr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr[i];
		if ( temp1 < temp2 )
		{
			arr[i] = temp1;
			j = i - 1;
			while (j > left)
			{
				temp = arr[j-1];
				if ( temp < temp2 )
					arr[j--] = temp;
				else
					break;
			}
			arr[j] = temp2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}

extern "C" void InsertionSort_SortInt64Inv(Int64 *arr, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	Int64 temp;
	Int64 temp1;
	Int64 temp2;
	temp1 = arr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr[i];
		if ( temp1 < temp2 )
		{
			arr[i] = temp1;
			j = i - 1;
			while (j > left)
			{
				temp = arr[j-1];
				if ( temp < temp2 )
					arr[j--] = temp;
				else
					break;
			}
			arr[j] = temp2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}

extern "C" void InsertionSort_SortSingleInv(Single *arr, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	Single temp;
	Single temp1;
	Single temp2;
	temp1 = arr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr[i];
		if ( temp1 < temp2 )
		{
			arr[i] = temp1;
			j = i - 1;
			while (j > left)
			{
				temp = arr[j-1];
				if ( temp < temp2 )
					arr[j--] = temp;
				else
					break;
			}
			arr[j] = temp2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}

extern "C" void InsertionSort_SortDoubleInv(Double *arr, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	Double temp;
	Double temp1;
	Double temp2;
	temp1 = arr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr[i];
		if ( temp1 < temp2 )
		{
			arr[i] = temp1;
			j = i - 1;
			while (j > left)
			{
				temp = arr[j-1];
				if ( temp < temp2 )
					arr[j--] = temp;
				else
					break;
			}
			arr[j] = temp2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}

extern "C" void InsertionSort_SortCmpInv(void **arr, Data::IComparable::CompareFunc func, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	void *temp;
	void *temp1;
	void *temp2;
	temp1 = arr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr[i];
		if (func(temp1, temp2) < 0)
		{
			arr[i] = temp1;
			j = i - 1;
			while (j > left)
			{
				temp = arr[j-1];
				if (func(temp, temp2) < 0)
					arr[j--] = temp;
				else
					break;
			}
			arr[j] = temp2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}
