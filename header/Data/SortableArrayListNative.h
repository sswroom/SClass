#ifndef _SM_DATA_SORTABLEARRAYLISTNATIVE
#define _SM_DATA_SORTABLEARRAYLISTNATIVE
#include "Data/ArrayList.h"
#include "Data/Comparator.h"
#include "Data/Sort/ArtificialQuickSortCmp.h"

namespace Data
{
	template <class T> class SortableArrayListNative : public Data::ArrayList<T>, public Data::Comparator<T>
	{
	public:
		SortableArrayListNative() : ArrayList<T>() {};
		SortableArrayListNative(UOSInt capacity) : ArrayList<T>() {};
		virtual ~SortableArrayListNative() {};

		UOSInt SortedInsert(T val);
		OSInt SortedIndexOf(T val) const;
		virtual OSInt Compare(T a, T b) const;
		T Min() const;
		T Max() const;
		Double Mean() const;
		T Median() const;
	};

	template <class T> UOSInt Data::SortableArrayListNative<T>::SortedInsert(T val)
	{
		OSInt i;
		OSInt j;
		OSInt k;
		T l;
		i = 0;
		j = (OSInt)this->objCnt - 1;
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
		this->Insert((UOSInt)i, val);
		return (UOSInt)i;
	}

	template <class T> OSInt Data::SortableArrayListNative<T>::SortedIndexOf(T val) const
	{
		OSInt i;
		OSInt j;
		OSInt k;
		T l;
		i = 0;
		j = (OSInt)this->objCnt - 1;
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

	template <class T> OSInt Data::SortableArrayListNative<T>::Compare(T a, T b) const
	{
		if (a > b) return 1;
		if (a < b) return -1;
		return 0;
	}

	template <class T> T Data::SortableArrayListNative<T>::Min() const
	{
		UOSInt i = this->objCnt;
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
		UOSInt i = this->objCnt;
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
		UOSInt i = this->objCnt;
		while (i-- > 0)
		{
			sum += (Double)this->arr[i];
		}
		return sum / (Double)this->objCnt;
	}

	template <class T> T Data::SortableArrayListNative<T>::Median() const
	{
		UOSInt cnt = this->objCnt;
		if (cnt == 0)
			return 0;
		if (cnt == 1)
			return this->arr[0];
		UnsafeArray<T> tmpArr = MemAllocArr(T, cnt);
		MemCopyNO(tmpArr.Ptr(), this->arr.Ptr(), cnt * sizeof(T));
		ArtificialQuickSort_Sort(tmpArr.Ptr(), 0, (OSInt)cnt - 1);
		T v = tmpArr[cnt >> 1];
		MemFreeArr(tmpArr);
		return v;
	}
}
#endif
