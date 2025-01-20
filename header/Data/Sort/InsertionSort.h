#ifndef _SM_DATA_SORT_INSERTIONSORT
#define _SM_DATA_SORT_INSERTIONSORT
#include "Data/Comparator.h"
#include "Data/Comparable.h"

extern "C"
{
	// Binary Search for larger arr
	void InsertionSort_SortBInt32(Int32 *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortBStr(UTF8Char **arr, OSInt firstIndex, OSInt lastIndex);

	// Sequencial Search for small arr
	void InsertionSort_SortInt32(Int32 *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortUInt32(UInt32 *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortInt64(Int64 *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortSingle(Single *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortDouble(Double *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortStr(UTF8Char **arr, OSInt firstIndex, OSInt lastIndex);

	void InsertionSort_SortInt32Inv(Int32 *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortUInt32Inv(UInt32 *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortInt64Inv(Int64 *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortSingleInv(Single *arr, OSInt firstIndex, OSInt lastIndex);
	void InsertionSort_SortDoubleInv(Double *arr, OSInt firstIndex, OSInt lastIndex);
}

namespace Data
{
	namespace Sort
	{
		class InsertionSort
		{
		public:
			template <class T> static void SortB(UnsafeArray<T> arr, NN<Data::Comparator<T>> comparator, OSInt firstIndex, OSInt lastIndex);
			template <class T, class V> static void SortBKV(UnsafeArray<T> keyArr, UnsafeArray<V> valArr, OSInt firstIndex, OSInt lastIndex);
			static void SortBCmpO(UnsafeArray<NN<Data::Comparable>> arr, OSInt firstIndex, OSInt lastIndex);
		};
	}
}

template <class T> void Data::Sort::InsertionSort::SortB(UnsafeArray<T> arr, NN<Data::Comparator<T>> comparator, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	T temp;
	T temp1;
	T temp2;
	temp1 = arr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = arr[i];
		if ( comparator->Compare(temp1, temp2) > 0)
		{
			j = left;
			k = i - 1;
			while (j <= k)
			{
				l = (j + k) >> 1;
				temp = arr[l];
				if (comparator->Compare(temp, temp2) > 0)
				{
					k = l - 1;
				}
				else
				{
					j = l + 1;
				}
			}
			MemCopyO(&arr[j + 1], &arr[j], (UOSInt)(i - j) * sizeof(arr[0]));
			arr[j] = temp2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}


template <class T, class V> void Data::Sort::InsertionSort::SortBKV(UnsafeArray<T> keyArr, UnsafeArray<V> valArr, OSInt left, OSInt right)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	T temp;
	T temp1;
	T temp2;
	V val2;
	temp1 = keyArr[left];
	i = left + 1;
	while (i <= right)
	{
		temp2 = keyArr[i];
		if ( temp1 > temp2)
		{
			val2 = valArr[i];
			j = left;
			k = i - 1;
			while (j <= k)
			{
				l = (j + k) >> 1;
				temp = keyArr[l];
				if (temp > temp2)
				{
					k = l - 1;
				}
				else
				{
					j = l + 1;
				}
			}
			MemCopyO(&keyArr[j + 1], &keyArr[j], (UOSInt)(i - j) * sizeof(keyArr[0]));
			MemCopyO(&valArr[j + 1], &valArr[j], (UOSInt)(i - j) * sizeof(valArr[0]));
			keyArr[j] = temp2;
			valArr[j] = val2;
		}
		else
		{
			temp1 = temp2;
		}
		i++;
	}
}
#endif
