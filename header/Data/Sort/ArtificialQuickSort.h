#ifndef _SM_DATA_SORT_ARTIFICIALQUICKSORT
#define _SM_DATA_SORT_ARTIFICIALQUICKSORT
#include "AnyType.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListNN.h"
#include "Data/Comparator.h"
#include "Data/Sort/ArtificialQuickSortC.h"
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
				ArtificialQuickSort *me;
				UOSInt threadId;
				Bool toStop;
				Int32 state; // 0 = not running, 1 = idle, 2 = processing
				Sync::Event *evt;
			} ThreadStat;

			typedef enum
			{
				AT_INT32,
				AT_UINT32,
				AT_STRUTF8
			} ArrayType;
		private:
			UOSInt threadCnt;
			ThreadStat *threads;
			Sync::Event mainEvt;

			Sync::Mutex mut;
			void *arr;
			ArrayType arrType;
			OSInt *tasks;
			UOSInt taskCnt;

			void DoSortInt32(NN<ThreadStat> stat, Int32 *arr, OSInt firstIndex, OSInt lastIndex);
			void DoSortUInt32(NN<ThreadStat> stat, UInt32 *arr, OSInt firstIndex, OSInt lastIndex);
			void DoSortStr(NN<ThreadStat> stat, UTF8Char **arr, OSInt firstIndex, OSInt lastIndex);
			static UInt32 __stdcall ProcessThread(AnyType userObj);
		public:
			ArtificialQuickSort();
			~ArtificialQuickSort();

			void SortInt32(Int32 *arr, OSInt firstIndex, OSInt lastIndex);
			void SortUInt32(UInt32 *arr, OSInt firstIndex, OSInt lastIndex);
			void SortStr(UTF8Char **arr, OSInt firstIndex, OSInt lastIndex);

			template <class T> static void PreSort(UnsafeArray<T> arr, NN<Data::Comparator<T>> comparator, OSInt firstIndex, OSInt lastIndex);
			template <class T> static void Sort(UnsafeArray<T> arr, NN<Data::Comparator<T>> comparator, OSInt firstIndex, OSInt lastIndex);
			template <class T> static void Sort(Data::ArrayCollection<T> *list, NN<Data::Comparator<T>> comparator);
		};
	}
}

template <class T> void Data::Sort::ArtificialQuickSort::PreSort(UnsafeArray<T> arr, NN<Data::Comparator<T>> comparator, OSInt left, OSInt right)
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

template <class T> void Data::Sort::ArtificialQuickSort::Sort(UnsafeArray<T> arr, NN<Data::Comparator<T>> comparator, OSInt firstIndex, OSInt lastIndex)
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
	T meja;
	OSInt left1;
	OSInt right1;
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
	MemFree(levi);
#endif
}

template <class T> void Data::Sort::ArtificialQuickSort::Sort(Data::ArrayCollection<T> *list, NN<Data::Comparator<T>> comparator)
{
	UOSInt len;
	UnsafeArray<T> arr = list->GetPtr(len);
	Sort(arr, comparator, 0, (OSInt)len - 1);
}
#endif
