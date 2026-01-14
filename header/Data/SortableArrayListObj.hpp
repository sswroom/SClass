#ifndef _SM_DATA_SORTABLEARRAYLISTOBJ
#define _SM_DATA_SORTABLEARRAYLISTOBJ
#include "Data/ArrayListObj.hpp"
#include "Data/Comparator.hpp"

namespace Data
{
	template <class T> class SortableArrayListObj : public Data::ArrayListObj<T>, public Comparator<T>
	{
	public:
		SortableArrayListObj() : ArrayListObj<T>() {};
		SortableArrayListObj(UOSInt capacity) : ArrayListObj<T>() {};
		UOSInt SortedInsert(T val);
		OSInt SortedIndexOf(T val) const;
	};

	template <class T> UOSInt Data::SortableArrayListObj<T>::SortedInsert(T val)
	{
		OSInt i;
		OSInt j;
		OSInt k;
		OSInt l;
		i = 0;
		j = (OSInt)this->objCnt - 1;
		while (i <= j)
		{
			k = (i + j) >> 1;
			l = this->Compare(this->arr[k], val);
			if (l > 0)
			{
				j = k - 1;
			}
			else if (l < 0)
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

	template <class T> OSInt Data::SortableArrayListObj<T>::SortedIndexOf(T val) const
	{
		OSInt i;
		OSInt j;
		OSInt k;
		OSInt l;
		i = 0;
		j = (OSInt)this->objCnt - 1;
		while (i <= j)
		{
			k = (i + j) >> 1;
			l = this->Compare(this->arr[k], val);
			if (l > 0)
			{
				j = k - 1;
			}
			else if (l < 0)
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
}
#endif
