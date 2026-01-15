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
		SortableArrayListObj(UIntOS capacity) : ArrayListObj<T>() {};
		UIntOS SortedInsert(T val);
		IntOS SortedIndexOf(T val) const;
	};

	template <class T> UIntOS Data::SortableArrayListObj<T>::SortedInsert(T val)
	{
		IntOS i;
		IntOS j;
		IntOS k;
		IntOS l;
		i = 0;
		j = (IntOS)this->objCnt - 1;
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
		this->Insert((UIntOS)i, val);
		return (UIntOS)i;
	}

	template <class T> IntOS Data::SortableArrayListObj<T>::SortedIndexOf(T val) const
	{
		IntOS i;
		IntOS j;
		IntOS k;
		IntOS l;
		i = 0;
		j = (IntOS)this->objCnt - 1;
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
