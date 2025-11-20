#ifndef _SM_DATA_ARRAYLISTT
#define _SM_DATA_ARRAYLISTT
#include "Data/ArrayListBase.hpp"

namespace Data
{
	template <class T> class ArrayListT : public ArrayListBase<T>
	{
	public:
		ArrayListT();
		virtual ~ArrayListT();

		virtual UOSInt Add(T val);
		virtual UOSInt AddRange(UnsafeArray<const T> arr, UOSInt cnt);
		virtual T RemoveAt(UOSInt index);
		virtual void Insert(UOSInt index, T val);
	};

	template <class T> ArrayListT<T>::ArrayListT()
	{
	}

	template <class T> ArrayListT<T>::~ArrayListT()
	{
	}

	template <class T> UOSInt ArrayListT<T>::Add(T val)
	{
		UOSInt ret;
		if (this->objCnt == this->capacity)
		{
			UnsafeArray<T> newArr = MemAllocArr(T, this->capacity << 1);
			UOSInt i = this->objCnt;
			while (i-- > 0)
			{
				newArr[i] = this->arr[i];
			}
			this->capacity = this->capacity << 1;
			MemFreeArr(this->arr);
			this->arr = newArr;
		}
		this->arr[ret = this->objCnt++] = val;
		return ret;
	}

	template <class T> UOSInt ArrayListT<T>::AddRange(UnsafeArray<const T> arr, UOSInt cnt)
	{
		UOSInt i;
		if (this->objCnt + cnt >= this->capacity)
		{
			while (this->objCnt + cnt >= this->capacity)
			{
				this->capacity = this->capacity << 1;
			}
			UnsafeArray<T> newArr = MemAllocArr(T, this->capacity);
			if (this->objCnt > 0)
			{
				i = this->objCnt;
				while (i-- > 0)
				{
					newArr[i] = this->arr[i];
				}
			}
			MemFreeArr(this->arr);
			this->arr = newArr;
		}
		i = cnt;
		while (i-- > 0)
		{
			this->arr[this->objCnt + i] = arr[i];
		}
		this->objCnt += cnt;
		return cnt;
	}

	template <class T> T ArrayListT<T>::RemoveAt(UOSInt index)
	{
		if (index >= this->objCnt)
			return (T)0;
		UOSInt i = index + 1;
		T o = this->arr[index];
		while (i < this->objCnt)
		{
			this->arr[i - 1] = this->arr[i];
			i++;
		}
		this->objCnt--;
		return o;
	}

	template <class T> void ArrayListT<T>::Insert(UOSInt index, T Val)
	{
		if (this->objCnt == this->capacity)
		{
			UOSInt i;
			UnsafeArray<T> newArr = MemAllocArr(T, this->capacity << 1);
			if (index > 0)
			{
				i = index;
				while (i-- > 0)
				{
					newArr[i] = this->arr[i];
				}
			}
			newArr[index] = Val;
			if (index < this->objCnt)
			{
				i = index;
				while (i < this->objCnt)
				{
					newArr[i + 1] = this->arr[i];
					i++;
				}
			}
			this->capacity = this->capacity << 1;
			MemFreeArr(this->arr);
			this->arr = newArr;
		}
		else
		{
			UOSInt j = this->objCnt;
			while (j > index)
			{
				this->arr[j] = this->arr[j - 1];
				j--;
			}
			this->arr[index] = Val;
		}
		this->objCnt++;
	}
}
#endif
