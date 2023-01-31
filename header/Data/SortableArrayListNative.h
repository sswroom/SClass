#ifndef _SM_DATA_SORTABLEARRAYLISTNATIVE
#define _SM_DATA_SORTABLEARRAYLISTNATIVE
#include "Data/ArrayList.h"
#include "Data/Comparator.h"

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
}
#endif
