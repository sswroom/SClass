#ifndef _SM_DATA_SORT_ARTIFICIALQUICKSORT
#define _SM_DATA_SORT_ARTIFICIALQUICKSORT
#include "AnyType.h"
#include "MyMemory.h"
#include "Data/ArrayCollection.hpp"
#include "Data/Comparator.hpp"
#include "Data/Sort/ArtificialQuickSort_C.h"
#include "Data/Sort/InsertionSort.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

//http://www.critticall.com/ArtificialSort.html

namespace Data
{
	namespace Sort
	{
		class ArtificialQuickSort
		{
		private:
			typedef struct
			{
				NN<ArtificialQuickSort> me;
				UIntOS threadId;
				Bool toStop;
				Int32 state; // 0 = not running, 1 = idle, 2 = processing
				NN<Sync::Event> evt;
			} ThreadStat;

			typedef enum
			{
				AT_INT32,
				AT_UINT32,
				AT_STRUTF8
			} ArrayType;
		private:
			UIntOS threadCnt;
			UnsafeArray<ThreadStat> threads;
			Sync::Event mainEvt;

			Sync::Mutex mut;
			UnsafeArrayOpt<UInt8> arr;
			ArrayType arrType;
			UnsafeArray<IntOS> tasks;
			UIntOS taskCnt;

			void DoSortInt32(NN<ThreadStat> stat, UnsafeArray<Int32> arr, IntOS firstIndex, IntOS lastIndex);
			void DoSortUInt32(NN<ThreadStat> stat, UnsafeArray<UInt32> arr, IntOS firstIndex, IntOS lastIndex);
			void DoSortStr(NN<ThreadStat> stat, UnsafeArray<UnsafeArray<UTF8Char>> arr, IntOS firstIndex, IntOS lastIndex);
			static UInt32 __stdcall ProcessThread(AnyType userObj);
		public:
			ArtificialQuickSort();
			~ArtificialQuickSort();

			void SortInt32(UnsafeArray<Int32> arr, IntOS firstIndex, IntOS lastIndex);
			void SortUInt32(UnsafeArray<UInt32> arr, IntOS firstIndex, IntOS lastIndex);
			void SortStr(UnsafeArray<UnsafeArray<UTF8Char>> arr, IntOS firstIndex, IntOS lastIndex);

			template <class T> static void PreSort(UnsafeArray<T> arr, NN<const Data::Comparator<T>> comparator, IntOS firstIndex, IntOS lastIndex);
			template <class T> static void Sort(UnsafeArray<T> arr, NN<const Data::Comparator<T>> comparator, IntOS firstIndex, IntOS lastIndex);
			template <class T> static void Sort(NN<Data::ArrayCollection<T>> list, NN<const Data::Comparator<T>> comparator);
			template <class T, class V> static void PreSortKV(UnsafeArray<T> keyArr, UnsafeArray<V> valArr, IntOS firstIndex, IntOS lastIndex);
			template <class T, class V> static void SortKV(UnsafeArray<T> keyArr, UnsafeArray<V> valArr, IntOS firstIndex, IntOS lastIndex);
			static void PreSortCmpO(UnsafeArray<NN<Data::Comparable>> arr, IntOS firstIndex, IntOS lastIndex);
			static void SortCmpO(UnsafeArray<NN<Data::Comparable>> arr, IntOS firstIndex, IntOS lastIndex);
		};
	}
}

template <class T> void Data::Sort::ArtificialQuickSort::PreSort(UnsafeArray<T> arr, NN<const Data::Comparator<T>> comparator, IntOS left, IntOS right)
{
	T temp = arr[left];
	T temp2;
	while (left < right)
	{
		temp = arr[left];
		temp2 = arr[right];
		if (comparator->Compare(temp, temp2) > 0)
		{
			arr[left] = temp2;
			arr[right] = temp;
		}
		left++;
		right--;
	}
}

template <class T> void Data::Sort::ArtificialQuickSort::Sort(UnsafeArray<T> arr, NN<const Data::Comparator<T>> comparator, IntOS firstIndex, IntOS lastIndex)
{
#if _OSINT_SIZE == 16
	IntOS levi[256];
	IntOS desni[256];
#else
	UnsafeArray<IntOS> levi = MemAllocArr(IntOS, 65536);
	UnsafeArray<IntOS> desni = &levi[32768];
#endif
	IntOS index;
	IntOS i;
	IntOS left;
	IntOS right;
	T meja;
	IntOS left1;
	IntOS right1;
	T temp;

	PreSort(arr, comparator, firstIndex, lastIndex);

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
			Data::Sort::InsertionSort::SortB(arr, comparator, left, right);
			index--;
		}
		else
		{
			meja = arr[ (left + right) >> 1 ];
			left1 = left;
			right1 = right;
			while (true)
			{
				while (comparator->Compare(arr[right1], meja) >= 0 )
				{
					if (--right1 < left1)
						break;
				}
				while (comparator->Compare(arr[left1], meja) < 0 )
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
	MemFreeArr(levi);
#endif
}

template <class T> void Data::Sort::ArtificialQuickSort::Sort(NN<Data::ArrayCollection<T>> list, NN<const Data::Comparator<T>> comparator)
{
	UIntOS len;
	UnsafeArray<T> arr = list->GetArr(len);
	Sort(arr, comparator, 0, (IntOS)len - 1);
}

template <class T, class V> void Data::Sort::ArtificialQuickSort::PreSortKV(UnsafeArray<T> keyArr, UnsafeArray<V> valArr, IntOS left, IntOS right)
{
	T temp = keyArr[left];
	T temp2;
	V v;
	while (left < right)
	{
		temp = keyArr[left];
		temp2 = keyArr[right];
		if (temp > temp2)
		{
			keyArr[left] = temp2;
			keyArr[right] = temp;
			v = valArr[left];
			valArr[left] = valArr[right];
			valArr[right] = v;
		}
		left++;
		right--;
	}
}

template <class T, class V> void Data::Sort::ArtificialQuickSort::SortKV(UnsafeArray<T> keyArr, UnsafeArray<V> valArr, IntOS firstIndex, IntOS lastIndex)
{
#if _OSINT_SIZE == 16
	IntOS levi[256];
	IntOS desni[256];
#else
	UnsafeArray<IntOS> levi = MemAllocArr(IntOS, 65536);
	UnsafeArray<IntOS> desni = &levi[32768];
#endif
	IntOS index;
	IntOS i;
	IntOS left;
	IntOS right;
	T meja;
	V mejaV;
	IntOS left1;
	IntOS right1;
	T temp;
	V tempV;

	PreSortKV(keyArr, valArr, firstIndex, lastIndex);

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
			Data::Sort::InsertionSort::SortBKV(keyArr, valArr, left, right);
			index--;
		}
		else
		{
			meja = keyArr[ (left + right) >> 1 ];
			mejaV = valArr[ (left + right) >> 1 ];
			left1 = left;
			right1 = right;
			while (true)
			{
				while (keyArr[right1] >= meja)
				{
					if (--right1 < left1)
						break;
				}
				while (keyArr[left1] < meja)
				{
					if (++left1 > right1)
						break;
				}
				if (left1 > right1)
					break;

				temp = keyArr[right1];
				tempV = valArr[right1];
				keyArr[right1] = keyArr[left1];
				valArr[right1--] = valArr[left1];
				keyArr[left1] = temp;
				valArr[left1++] = tempV;
			}
			if (left1 == left)
			{
				keyArr[(left + right) >> 1] = keyArr[left];
				valArr[(left + right) >> 1] = valArr[left];
				keyArr[left] = meja;
				valArr[left] = mejaV;
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
	MemFreeArr(levi);
#endif
}
#endif
