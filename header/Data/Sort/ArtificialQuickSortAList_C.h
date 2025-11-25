#ifndef _SM_DATA_SORT_ARTIFICIALQUICKSORTALIST_C
#define _SM_DATA_SORT_ARTIFICIALQUICKSORTALIST_C
#include "Data/ArrayList.hpp"
#include "Data/ArrayListNN.hpp"
#include "Data/Sort/ArtificialQuickSortCmp_C.h"

template <class T> void ArtificialQuickSort_SortAList(Data::ArrayListNN<T> *arr);
template <class T> void ArtificialQuickSort_SortAList(Data::ArrayList<T> *arr);

template <class T> void ArtificialQuickSort_SortAList(Data::ArrayList<T> *arr)
{
	UOSInt size;
	T *a = arr->GetArray(&size);
	ArtificialQuickSort_Sort(a, 0, (OSInt)size - 1);
}

template <class T> void ArtificialQuickSort_SortAList(Data::ArrayListNN<T> *arr)
{
	ArtificialQuickSort_Sort(arr->Arr().Ptr(), 0, (OSInt)arr->GetCount() - 1);
}


#endif
