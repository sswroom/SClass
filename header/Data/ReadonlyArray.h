#ifndef _SM_DATA_READONLYARRAY
#define _SM_DATA_READONLYARRAY
#include "Data/DataComparer.h"

namespace Data
{
	template <class T> class ReadonlyArray
	{
	protected:
		T *arr;
		UOSInt cnt;

	public:
		ReadonlyArray(const T *arr, UOSInt cnt);
		~ReadonlyArray();

		UOSInt GetCount() const;
		const T *GetArray() const;
		T GetItem(UOSInt index) const;
		T operator [](UOSInt index) const;
		NotNullPtr<ReadonlyArray<T>> Clone() const;
		Bool Equals(ReadonlyArray<T> *arr2) const;
	};

	template <class T> ReadonlyArray<T>::ReadonlyArray(const T *arr, UOSInt cnt)
	{
		this->arr = MemAlloc(T, cnt);
		MemCopyNO(this->arr, arr, cnt * sizeof(T));
		this->cnt = cnt;
	}

	template <class T> ReadonlyArray<T>::~ReadonlyArray()
	{
		MemFree(this->arr);
	}

	template <class T> UOSInt ReadonlyArray<T>::GetCount() const
	{
		return this->cnt;
	}

	template <class T> const T *ReadonlyArray<T>::GetArray() const
	{
		return this->arr;
	}

	template <class T> T ReadonlyArray<T>::GetItem(UOSInt index) const
	{
		return (*this)[index];
	}
	
	template <class T> T ReadonlyArray<T>::operator [](UOSInt index) const
	{
		if (index >= this->cnt)
		{
			return 0;
		}
		return this->arr[index];
	}

	template <class T> NotNullPtr<ReadonlyArray<T>> ReadonlyArray<T>::Clone() const
	{
		NotNullPtr<ReadonlyArray<T>> ret;
		NEW_CLASSNN(ret, ReadonlyArray<T>(this->arr, this->cnt));
		return ret;
	}

	template <class T> Bool ReadonlyArray<T>::Equals(ReadonlyArray<T> *arr2) const
	{
		if (arr2 == 0)
		{
			return false;
		}
		if (this->cnt != arr2->cnt)
		{
			return false;
		}
		UOSInt i = this->cnt;
		while (i-- > 0)
		{
			if (Data::DataComparer::Compare(this->arr[i], arr2->arr[i])	!= 0)
			{
				return false;
			}
		}
		return true;
	}
}
#endif
