#ifndef _SM_DATA_SORTABLEARRAYLISTNN
#define _SM_DATA_SORTABLEARRAYLISTNN
#include "Data/ArrayListNN.hpp"
#include "Data/Comparator.hpp"

namespace Data
{
	template <class T> class SortableArrayListNN : public Data::ArrayListNN<T>, public Comparator<NN<T>>
	{
	public:
		SortableArrayListNN() : ArrayListNN<T>() {};
		SortableArrayListNN(UIntOS capacity) : ArrayListNN<T>() {};

		UIntOS SortedInsert(NN<T> val);
		IntOS SortedIndexOf(NN<T> val) const;
	};

	template <class T> UIntOS Data::SortableArrayListNN<T>::SortedInsert(NN<T> val)
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

	template <class T> IntOS Data::SortableArrayListNN<T>::SortedIndexOf(NN<T> val) const
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
