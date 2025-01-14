#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/IComparable.h"
#include "Data/Sort/InsertionSort.h"
#include "Text/MyString.h"

extern "C" void ArtificialQuickSort_PreSortInt32(Int32 *arr, OSInt left, OSInt right)
{
	Int32 temp = 0;
	Int32 temp2;

	while (left < right)
	{
		temp = arr[left];
		temp2 = arr[right];
		if (temp > temp2)
		{
			arr[left] = temp2;
			arr[right] = temp;
		}
		left++;
		right--;
	}
}

extern "C" void ArtificialQuickSort_PreSortUInt32(UInt32 *arr, OSInt left, OSInt right)
{
	UInt32 temp = 0;

	while (left < right)
	{
		if (arr[left] > arr[right])
		{
			temp = arr[left];
			arr[left] = arr[right];
			arr[right] = temp;
		}
		left++;
		right--;
	}
}

extern "C" void ArtificialQuickSort_PreSortInt64(Int64 *arr, OSInt left, OSInt right)
{
	Int64 temp = 0;

	while (left < right)
	{
		if (arr[left] > arr[right])
		{
			temp = arr[left];
			arr[left] = arr[right];
			arr[right] = temp;
		}
		left++;
		right--;
	}
}

extern "C" void ArtificialQuickSort_PreSortSingle(Single *arr, OSInt left, OSInt right)
{
	Single temp = 0;

	while (left < right)
	{
		if (arr[left] > arr[right])
		{
			temp = arr[left];
			arr[left] = arr[right];
			arr[right] = temp;
		}
		left++;
		right--;
	}
}

extern "C" void ArtificialQuickSort_PreSortDouble(Double *arr, OSInt left, OSInt right)
{
	Double temp = 0;

	while (left < right)
	{
		if (arr[left] > arr[right])
		{
			temp = arr[left];
			arr[left] = arr[right];
			arr[right] = temp;
		}
		left++;
		right--;
	}
}

extern "C" void ArtificialQuickSort_PreSortStr(UTF8Char **arr, OSInt left, OSInt right)
{
	UTF8Char *temp = 0;
	UTF8Char *temp2;

	while (left < right)
	{
		temp = arr[left];
		temp2 = arr[right];
		if (Text::StrCompare(temp, temp2) > 0)
		{
			arr[left] = temp2;
			arr[right] = temp;
		}
		left++;
		right--;
	}
}

extern "C" void ArtificialQuickSort_PreSortInt32Inv(Int32 *arr, OSInt left, OSInt right)
{
	Int32 temp = 0;

	while (left < right)
	{
		if (arr[left] < arr[right])
		{
			temp = arr[left];
			arr[left] = arr[right];
			arr[right] = temp;
		}
		left++;
		right--;
	}
}

extern "C" void ArtificialQuickSort_PreSortUInt32Inv(UInt32 *arr, OSInt left, OSInt right)
{
	UInt32 temp = 0;

	while (left < right)
	{
		if (arr[left] < arr[right])
		{
			temp = arr[left];
			arr[left] = arr[right];
			arr[right] = temp;
		}
		left++;
		right--;
	}
}

extern "C" void ArtificialQuickSort_PreSortInt64Inv(Int64 *arr, OSInt left, OSInt right)
{
	Int64 temp = 0;

	while (left < right)
	{
		if (arr[left] < arr[right])
		{
			temp = arr[left];
			arr[left] = arr[right];
			arr[right] = temp;
		}
		left++;
		right--;
	}
}

extern "C" void ArtificialQuickSort_PreSortSingleInv(Single *arr, OSInt left, OSInt right)
{
	Single temp = 0;

	while (left < right)
	{
		if (arr[left] < arr[right])
		{
			temp = arr[left];
			arr[left] = arr[right];
			arr[right] = temp;
		}
		left++;
		right--;
	}
}

extern "C" void ArtificialQuickSort_PreSortDoubleInv(Double *arr, OSInt left, OSInt right)
{
	Double temp = 0;

	while (left < right)
	{
		if (arr[left] < arr[right])
		{
			temp = arr[left];
			arr[left] = arr[right];
			arr[right] = temp;
		}
		left++;
		right--;
	}
}

extern "C" void ArtificialQuickSort_SortInt32(Int32 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt levi[256];
	OSInt desni[256];
	OSInt index;
	OSInt i;
	OSInt j;
	OSInt left;
	OSInt right;
	Int32 meja;
	OSInt left1;
	OSInt right1;
	Int32 temp1;
	Int32 temp2;
	Int32 temp;

	ArtificialQuickSort_PreSortInt32(arr, firstIndex, lastIndex);

	index = 0;
	levi[index] = firstIndex;
	desni[index] = lastIndex;

	while ( index >= 0 )
	{
		left = levi[index];
		right = desni[index];
		i = right - left;
		if (i <= 0)
		{
		}
		else if (i <= 64)
		{
			InsertionSort_SortInt32(arr, left, right);
		}
		else
		{
			temp1 = arr[left];
			j = left + 1;
			while (j <= right)
			{
				temp2 = arr[j];
				if ( temp2 < temp1)
				{
					meja = (temp1 >> 1) + (temp2 >> 1) + ((temp1 & 1) | (temp2 & 1));
					if ( arr[ (left + right) >> 1 ] > meja )
					{
						meja = arr[ (left + right) >> 1 ];
					}
					left1 = left;
					right1 = right;
					while (true)
					{
						while ( arr[right1] >= meja ) right1--;
						while ( arr[left1] < meja ) left1++;
						if (left1 > right1)
							break;

						temp = arr[right1];
						arr[right1--] = arr[left1];
						arr[left1++] = temp;
					}
					if (left1 == left)
					{
						arr[(left + right) >> 1] = arr[left];
						arr[left] = meja;
						levi[index] = left + 1;
						desni[index] = right;
					}
					else
					{
						desni[index] = --left1;
						right1++;
						index++;
						levi[index] = right1;
						desni[index] = right;
					}
					goto labelcritticall5;
				}
				temp1 = temp2;
				j++;
			}
		}
		index--;
labelcritticall5:;
	}
}

extern "C" void ArtificialQuickSort_SortUInt32(UInt32 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt levi[256];
	OSInt desni[256];

	OSInt index;
	OSInt i;
	OSInt j;
	OSInt left;
	OSInt right;
	UInt32 meja;
	OSInt left1;
	OSInt right1;
	UInt32 temp1;
	UInt32 temp2;
	UInt32 temp;

	ArtificialQuickSort_PreSortUInt32(arr, firstIndex, lastIndex);

	index = 0;
	levi[index] = firstIndex;
	desni[index] = lastIndex;

	while (index >= 0)
	{
		left = levi[index];
		right = desni[index];
		i = right - left;
		if (i <= 0)
		{
		}
		else if (i <= 64)
		{
			InsertionSort_SortUInt32(arr, left, right);
		}
		else
		{
			temp1 = arr[left];
			j = left + 1;
			while (j <= right)
			{
				temp2 = arr[j];
				if ( temp2 < temp1)
				{
					meja = ( temp1 + temp2 + 1 ) >> 1;
					if ( arr[ (left + right) >> 1 ] > meja )
					{
						meja = arr[ (left + right) >> 1 ];
					}
					left1 = left;
					right1 = right;
					while (true)
					{
						while ( arr[right1] >= meja ) right1--;
						while ( arr[left1] < meja ) left1++;
						if (left1 > right1)
							break;

						temp = arr[right1];
						arr[right1--] = arr[left1];
						arr[left1++] = temp;
					}
					if (left1 == left)
					{
						arr[(left + right) >> 1] = arr[left];
						arr[left] = meja;
						levi[index] = left + 1;
						desni[index] = right;
					}
					else
					{
						desni[index] = --left1;
						right1++;
						index++;
						levi[index] = right1;
						desni[index] = right;
					}
					goto labelcritticall5;
				}
				temp1 = temp2;
				j++;
			}
		}
		index--;
labelcritticall5:;
	}
}

extern "C" void ArtificialQuickSort_SortInt64(Int64 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt levi[256];
	OSInt desni[256];

	OSInt index;
	OSInt i;
	OSInt j;
	OSInt left;
	OSInt right;
	Int64 meja;
	OSInt left1;
	OSInt right1;
	Int64 temp1;
	Int64 temp2;
	Int64 temp;

	ArtificialQuickSort_PreSortInt64(arr, firstIndex, lastIndex);

	index = 0;
	levi[index] = firstIndex;
	desni[index] = lastIndex;

	while ( index >= 0 )
	{
		left = levi[index];
		right = desni[index];
		i = right - left;
		if (i <= 0)
		{
		}
		else if (i <= 15)
		{
			InsertionSort_SortInt64(arr, left, right);
		}
		else
		{
			temp1 = arr[left];
			j = left + 1;
			while (j <= right)
			{
				temp2 = arr[j];
				if ( temp2 < temp1)
				{
					meja = (temp1 >> 1) + (temp2 >> 1) + ((temp1 & 1) | (temp2 & 1));
					if ( arr[ (left + right) >> 1 ] > meja )
					{
						meja = arr[ (left + right) >> 1 ];
					}
					left1 = left;
					right1 = right;
					while (true)
					{
						while ( arr[right1] >= meja ) right1--;
						while ( arr[left1] < meja ) left1++;
						if (left1 > right1)
							break;

						temp = arr[right1];
						arr[right1--] = arr[left1];
						arr[left1++] = temp;
					}
					desni[index] = --left1;
					levi[++index] = ++right1;
					desni[index] = right;
					goto labelcritticall5;
				}
				temp1 = temp2;
				j++;
			}
		}
		index--;
labelcritticall5:;
	}
}

extern "C" void ArtificialQuickSort_SortSingle(Single *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt levi[256];
	OSInt desni[256];

	OSInt index;
	OSInt i;
	OSInt j;
	OSInt left;
	OSInt right;
	Single meja;
	OSInt left1;
	OSInt right1;
	Single temp1;
	Single temp2;
	Single temp;

	ArtificialQuickSort_PreSortSingle(arr, firstIndex, lastIndex);

	index = 0;
	levi[index] = firstIndex;
	desni[index] = lastIndex;

	while (index >= 0)
	{
		left = levi[index];
		right = desni[index];
		i = right - left;
		if (i <= 0)
		{
		}
		else if (i <= 15)
		{
			InsertionSort_SortSingle(arr, left, right);
		}
		else
		{
			temp1 = arr[left];
			j = left + 1;
			while (j <= right)
			{
				temp2 = arr[j];
				if ( temp2 < temp1)
				{
					meja = ( temp1 + temp2 ) * 0.5F;
					if ( arr[ (left + right) >> 1 ] > meja )
					{
						meja = arr[ (left + right) >> 1 ];
					}
					left1 = left;
					right1 = right;
					while (true)
					{
						while ( arr[right1] >= meja ) right1--;
						while ( arr[left1] < meja ) left1++;
						if (left1 > right1)
							break;

						temp = arr[right1];
						arr[right1--] = arr[left1];
						arr[left1++] = temp;
					}
					desni[index] = --left1;
					levi[++index] = ++right1;
					desni[index] = right;
					goto labelcritticall5;
				}
				temp1 = temp2;
				j++;
			}
		}
		index--;
labelcritticall5:;
	}
}

extern "C" void ArtificialQuickSort_SortDouble(Double *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt levi[256];
	OSInt desni[256];

	OSInt index;
	OSInt i;
	OSInt j;
	OSInt left;
	OSInt right;
	Double meja;
	OSInt left1;
	OSInt right1;
	Double temp1;
	Double temp2;
	Double temp;

	ArtificialQuickSort_PreSortDouble(arr, firstIndex, lastIndex);

	index = 0;
	levi[index] = firstIndex;
	desni[index] = lastIndex;

	while (index >= 0)
	{
		left = levi[index];
		right = desni[index];
		i = right - left;
		if (i <= 0)
		{
		}
		else if (i <= 15)
		{
			InsertionSort_SortDouble(arr, left, right);
		}
		else
		{
			temp1 = arr[left];
			j = left + 1;
			while (j <= right)
			{
				temp2 = arr[j];
				if ( temp2 < temp1)
				{
					meja = ( temp1 + temp2 ) * 0.5;
					if ( arr[ (left + right) >> 1 ] > meja )
					{
						meja = arr[ (left + right) >> 1 ];
					}
					left1 = left;
					right1 = right;
					while (true)
					{
						while ( arr[right1] >= meja ) right1--;
						while ( arr[left1] < meja ) left1++;
						if (left1 > right1)
							break;

						temp = arr[right1];
						arr[right1--] = arr[left1];
						arr[left1++] = temp;
					}
					desni[index] = --left1;
					levi[++index] = ++right1;
					desni[index] = right;
					goto labelcritticall5;
				}
				temp1 = temp2;
				j++;
			}
		}
		index--;
labelcritticall5:;
	}
}

extern "C" void ArtificialQuickSort_SortStr(UTF8Char **arr, OSInt firstIndex, OSInt lastIndex)
{
#if _OSINT_SIZE == 16
	OSInt levi[256];
	OSInt desni[256];
#else
	OSInt *levi = MemAlloc(OSInt, 65536);
	OSInt *desni = &levi[32768];
#endif
	OSInt index;
	OSInt i;
	OSInt left;
	OSInt right;
	UTF8Char *meja;
	OSInt left1;
	OSInt right1;
	UTF8Char *temp;

	ArtificialQuickSort_PreSortStr(arr, firstIndex, lastIndex);

	index = 0;
	levi[index] = firstIndex;
	desni[index] = lastIndex;

	while ( index >= 0 )
	{
		left = levi[index];
		right = desni[index];
		i = right - left;
		if (i <= 0)
		{
			index--;
		}
		else if (i <= 64)
		{
			InsertionSort_SortBStr(arr, left, right);
			index--;
		}
		else
		{
			meja = arr[ (left + right) >> 1 ];
			left1 = left;
			right1 = right;
			while (true)
			{
				while ( Text::StrCompare(arr[right1], meja) >= 0 )
				{
					if (--right1 < left1)
						break;
				}
				while ( Text::StrCompare(arr[left1], meja) < 0 )
				{
					if (++left1 > right1)
						break;
				}
				if (left1 > right1)
					break;

				temp = arr[right1];
				arr[right1--] = arr[left1];
				arr[left1++] = temp;
			}
			if (left1 == left)
			{
				arr[(left + right) >> 1] = arr[left];
				arr[left] = meja;
				levi[index] = left + 1;
				desni[index] = right;
			}
			else
			{
				desni[index] = --left1;
				right1++;
				index++;
				levi[index] = right1;
				desni[index] = right;
			}
		}
	}
#if _OSINT_SIZE != 16
	MemFree(levi);
#endif
}

extern "C" void ArtificialQuickSort_SortInt32Inv(Int32 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt levi[256];
	OSInt desni[256];
	OSInt index;
	OSInt i;
	OSInt j;
	OSInt left;
	OSInt right;
	Int32 meja;
	OSInt left1;
	OSInt right1;
	Int32 temp1;
	Int32 temp2;
	Int32 temp;

	ArtificialQuickSort_PreSortInt32Inv(arr, firstIndex, lastIndex);

	index = 0;
	levi[index] = firstIndex;
	desni[index] = lastIndex;

	while ( index >= 0 )
	{
		left = levi[index];
		right = desni[index];
		i = right - left;
		if (i <= 0)
		{
		}
		else if (i <= 15)
		{
			InsertionSort_SortInt32Inv(arr, left, right);
		}
		else
		{
			temp1 = arr[left];
			j = left + 1;
			while (j <= right)
			{
				temp2 = arr[j];
				if ( temp2 > temp1)
				{
					meja = ( temp1 + temp2 + 1 ) >> 1;
					if ( arr[ (left + right) >> 1 ] < meja )
					{
						meja = arr[ (left + right) >> 1 ];
					}
					left1 = left;
					right1 = right;
					while (true)
					{
						while ( arr[right1] <= meja ) right1--;
						while ( arr[left1] > meja ) left1++;
						if (left1 > right1)
							break;

						temp = arr[right1];
						arr[right1--] = arr[left1];
						arr[left1++] = temp;
					}
					desni[index] = --left1;
					if (right1 + 1 < right)
					{
						levi[++index] = ++right1;
						desni[index] = right;
					}
					goto labelcritticall5;
				}
				temp1 = temp2;
				j++;
			}
		}
		index--;
labelcritticall5:;
	}
}

extern "C" void ArtificialQuickSort_SortUInt32Inv(UInt32 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt levi[256];
	OSInt desni[256];

	OSInt index;
	OSInt i;
	OSInt j;
	OSInt left;
	OSInt right;
	UInt32 meja;
	OSInt left1;
	OSInt right1;
	UInt32 temp1;
	UInt32 temp2;
	UInt32 temp;

	ArtificialQuickSort_PreSortUInt32Inv(arr, firstIndex, lastIndex);

	index = 0;
	levi[index] = firstIndex;
	desni[index] = lastIndex;

	while (index >= 0)
	{
		left = levi[index];
		right = desni[index];
		i = right - left;
		if (i <= 0)
		{
		}
		else if (i <= 15)
		{
			InsertionSort_SortUInt32Inv(arr, left, right);
		}
		else
		{
			temp1 = arr[left];
			j = left + 1;
			while (j <= right)
			{
				temp2 = arr[j];
				if ( temp2 > temp1)
				{
					meja = ( temp1 + temp2 + 1 ) >> 1;
					if ( arr[ (left + right) >> 1 ] < meja )
					{
						meja = arr[ (left + right) >> 1 ];
					}
					left1 = left;
					right1 = right;
					while (true)
					{
						while ( arr[right1] <= meja ) right1--;
						while ( arr[left1] > meja ) left1++;
						if (left1 > right1)
							break;

						temp = arr[right1];
						arr[right1--] = arr[left1];
						arr[left1++] = temp;
					}
					desni[index] = --left1;
					levi[++index] = ++right1;
					desni[index] = right;
					goto labelcritticall5;
				}
				temp1 = temp2;
				j++;
			}
		}
		index--;
labelcritticall5:;
	}
}

extern "C" void ArtificialQuickSort_SortInt64Inv(Int64 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt levi[256];
	OSInt desni[256];

	OSInt index;
	OSInt i;
	OSInt j;
	OSInt left;
	OSInt right;
	Int64 meja;
	OSInt left1;
	OSInt right1;
	Int64 temp1;
	Int64 temp2;
	Int64 temp;

	ArtificialQuickSort_PreSortInt64Inv(arr, firstIndex, lastIndex);

	index = 0;
	levi[index] = firstIndex;
	desni[index] = lastIndex;

	while ( index >= 0 )
	{
		left = levi[index];
		right = desni[index];
		i = right - left;
		if (i <= 0)
		{
		}
		else if (i <= 15)
		{
			InsertionSort_SortInt64Inv(arr, left, right);
		}
		else
		{
			temp1 = arr[left];
			j = left + 1;
			while (j <= right)
			{
				temp2 = arr[j];
				if ( temp2 > temp1)
				{
					meja = ( temp1 + temp2 + 1 ) >> 1;
					if ( arr[ (left + right) >> 1 ] < meja )
					{
						meja = arr[ (left + right) >> 1 ];
					}
					left1 = left;
					right1 = right;
					while (true)
					{
						while ( arr[right1] <= meja ) right1--;
						while ( arr[left1] > meja ) left1++;
						if (left1 > right1)
							break;

						temp = arr[right1];
						arr[right1--] = arr[left1];
						arr[left1++] = temp;
					}
					desni[index] = --left1;
					levi[++index] = ++right1;
					desni[index] = right;
					goto labelcritticall5;
				}
				temp1 = temp2;
				j++;
			}
		}
		index--;
labelcritticall5:;
	}
}

extern "C" void ArtificialQuickSort_SortSingleInv(Single *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt levi[256];
	OSInt desni[256];

	OSInt index;
	OSInt i;
	OSInt j;
	OSInt left;
	OSInt right;
	Single meja;
	OSInt left1;
	OSInt right1;
	Single temp1;
	Single temp2;
	Single temp;

	ArtificialQuickSort_PreSortSingleInv(arr, firstIndex, lastIndex);

	index = 0;
	levi[index] = firstIndex;
	desni[index] = lastIndex;

	while (index >= 0)
	{
		left = levi[index];
		right = desni[index];
		i = right - left;
		if (i <= 0)
		{
		}
		else if (i <= 15)
		{
			InsertionSort_SortSingleInv(arr, left, right);
		}
		else
		{
			temp1 = arr[left];
			j = left + 1;
			while (j <= right)
			{
				temp2 = arr[j];
				if ( temp2 > temp1)
				{
					meja = ( temp1 + temp2 ) * 0.5F;
					if ( arr[ (left + right) >> 1 ] < meja )
					{
						meja = arr[ (left + right) >> 1 ];
					}
					left1 = left;
					right1 = right;
					while (true)
					{
						while ( arr[right1] <= meja ) right1--;
						while ( arr[left1] > meja ) left1++;
						if (left1 > right1)
							break;

						temp = arr[right1];
						arr[right1--] = arr[left1];
						arr[left1++] = temp;
					}
					desni[index] = --left1;
					levi[++index] = ++right1;
					desni[index] = right;
					goto labelcritticall5;
				}
				temp1 = temp2;
				j++;
			}
		}
		index--;
labelcritticall5:;
	}
}

extern "C" void ArtificialQuickSort_SortDoubleInv(Double *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt levi[256];
	OSInt desni[256];

	OSInt index;
	OSInt i;
	OSInt j;
	OSInt left;
	OSInt right;
	Double meja;
	OSInt left1;
	OSInt right1;
	Double temp1;
	Double temp2;
	Double temp;

	ArtificialQuickSort_PreSortDoubleInv(arr, firstIndex, lastIndex);

	index = 0;
	levi[index] = firstIndex;
	desni[index] = lastIndex;

	while (index >= 0)
	{
		left = levi[index];
		right = desni[index];
		i = right - left;
		if (i <= 0)
		{
		}
		else if (i <= 15)
		{
			InsertionSort_SortDoubleInv(arr, left, right);
		}
		else
		{
			temp1 = arr[left];
			j = left + 1;
			while (j <= right)
			{
				temp2 = arr[j];
				if ( temp2 > temp1)
				{
					meja = ( temp1 + temp2 ) * 0.5;
					if ( arr[ (left + right) >> 1 ] < meja )
					{
						meja = arr[ (left + right) >> 1 ];
					}
					left1 = left;
					right1 = right;
					while (true)
					{
						while ( arr[right1] <= meja ) right1--;
						while ( arr[left1] > meja ) left1++;
						if (left1 > right1)
							break;

						temp = arr[right1];
						arr[right1--] = arr[left1];
						arr[left1++] = temp;
					}
					desni[index] = --left1;
					levi[++index] = ++right1;
					desni[index] = right;
					goto labelcritticall5;
				}
				temp1 = temp2;
				j++;
			}
		}
		index--;
labelcritticall5:;
	}
}

