#ifndef _SM_DATA_SORTABLEARRAYLISTNATIVE
#define _SM_DATA_SORTABLEARRAYLISTNATIVE
#include "Data/ArrayListNative.hpp"
#include "Data/Comparator.hpp"
#include "Data/Sort/ArtificialQuickSortCmp_C.h"

namespace Data
{
	template <class T> class SortableArrayListNative : public Data::ArrayListNative<T>, public Data::Comparator<T>
	{
	public:
		SortableArrayListNative() : ArrayListNative<T>() {};
		SortableArrayListNative(UIntOS capacity) : ArrayListNative<T>() {};
		virtual ~SortableArrayListNative() {};

		UIntOS SortedInsert(T val);
		IntOS SortedIndexOf(T val) const;
		virtual IntOS Compare(T a, T b) const;
		T Min() const;
		T Max() const;
		Double Mean() const;
		T Median() const;
	};

	template <class T> UIntOS Data::SortableArrayListNative<T>::SortedInsert(T val)
	{
		IntOS i;
		IntOS j;
		IntOS k;
		T l;
		i = 0;
		j = (IntOS)this->objCnt - 1;
		while (i <= j)
		{
			k = (i + j) >> 1;
			l = this->arr[k];
			if (l > val)
			{
				j = k - 1;
			}
			else if (l < val)
			{
				i = k + 1;
			}
			else
			{
				i = k + 1;
				break;
			}
		}
		this->Insert((UIntOS)i, val);
		return (UIntOS)i;
	}

	template <class T> IntOS Data::SortableArrayListNative<T>::SortedIndexOf(T val) const
	{
		IntOS i;
		IntOS j;
		IntOS k;
		T l;
		i = 0;
		j = (IntOS)this->objCnt - 1;
		while (i <= j)
		{
			k = (i + j) >> 1;
			l = this->arr[k];
			if (l > val)
			{
				j = k - 1;
			}
			else if (l < val)
			{
				i = k + 1;
			}
			else
			{
				return k;
			}
		}
		return -i - 1;
	}

	template <class T> IntOS Data::SortableArrayListNative<T>::Compare(T a, T b) const
	{
		if (a > b) return 1;
		if (a < b) return -1;
		return 0;
	}

	template <class T> T Data::SortableArrayListNative<T>::Min() const
	{
		UIntOS i = this->objCnt;
		if (this->objCnt == 0)
			return 0;
		T v = this->arr[0];
		while (i-- > 1)
		{
			if (this->arr[i] < v)
			{
				v = this->arr[i];
			}
		}
		return v;
	}

	template <class T> T Data::SortableArrayListNative<T>::Max() const
	{
		UIntOS i = this->objCnt;
		if (this->objCnt == 0)
			return 0;
		T v = this->arr[0];
		while (i-- > 1)
		{
			if (this->arr[i] > v)
			{
				v = this->arr[i];
			}
		}
		return v;
	}

	template <class T> Double Data::SortableArrayListNative<T>::Mean() const
	{
		Double sum = 0;
		UIntOS i = this->objCnt;
		while (i-- > 0)
		{
			sum += (Double)this->arr[i];
		}
		return sum / (Double)this->objCnt;
	}

	template <class T> T Data::SortableArrayListNative<T>::Median() const
	{
		UIntOS cnt = this->objCnt;
		if (cnt == 0)
			return 0;
		if (cnt == 1)
			return this->arr[0];
		UnsafeArray<T> tmpArr = MemAllocArr(T, cnt);
		MemCopyNO(tmpArr.Ptr(), this->arr.Ptr(), cnt * sizeof(T));
		ArtificialQuickSort_Sort(tmpArr.Ptr(), 0, (IntOS)cnt - 1);
		T v = tmpArr[cnt >> 1];
		MemFreeArr(tmpArr);
		return v;
	}
}
#endif
