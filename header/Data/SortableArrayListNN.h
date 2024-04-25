#ifndef _SM_DATA_SORTABLEARRAYLISTNN
#define _SM_DATA_SORTABLEARRAYLISTNN
#include "Data/ArrayListNN.h"
#include "Data/Comparator.h"

namespace Data
{
	template <class T> class SortableArrayListNN : public Data::ArrayListNN<T>, public Comparator<NN<T>>
	{
	public:
		SortableArrayListNN() : ArrayListNN<T>() {};
		SortableArrayListNN(UOSInt capacity) : ArrayListNN<T>() {};

		UOSInt SortedInsert(NN<T> val);
		OSInt SortedIndexOf(NN<T> val) const;
	};

	template <class T> UOSInt Data::SortableArrayListNN<T>::SortedInsert(NN<T> val)
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

	template <class T> OSInt Data::SortableArrayListNN<T>::SortedIndexOf(NN<T> val) const
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
