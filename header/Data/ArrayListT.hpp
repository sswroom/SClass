#ifndef _SM_DATA_ARRAYLISTT
#define _SM_DATA_ARRAYLISTT
#include "Data/ArrayListNativeBase.hpp"

namespace Data
{
	// ArrayList of class/struct which cannot use MemCopy
	template <class T> class ArrayListT : public ArrayListNativeBase<T>
	{
	public:
		ArrayListT();
		virtual ~ArrayListT();

		virtual UIntOS Add(T val);
		virtual UIntOS AddRange(UnsafeArray<const T> arr, UIntOS cnt);
		virtual T RemoveAt(UIntOS index);
		virtual void Insert(UIntOS index, T val);
	};

	template <class T> ArrayListT<T>::ArrayListT()
	{
	}

	template <class T> ArrayListT<T>::~ArrayListT()
	{
	}

	template <class T> UIntOS ArrayListT<T>::Add(T val)
	{
		UIntOS ret;
		if (this->objCnt == this->capacity)
		{
			UnsafeArray<T> newArr = MemAllocArr(T, this->capacity << 1);
			UIntOS i = this->objCnt;
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

	template <class T> UIntOS ArrayListT<T>::AddRange(UnsafeArray<const T> arr, UIntOS cnt)
	{
		UIntOS i;
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

	template <class T> T ArrayListT<T>::RemoveAt(UIntOS index)
	{
		if (index >= this->objCnt)
			return (T)0;
		UIntOS i = index + 1;
		T o = this->arr[index];
		while (i < this->objCnt)
		{
			this->arr[i - 1] = this->arr[i];
			i++;
		}
		this->objCnt--;
		return o;
	}

	template <class T> void ArrayListT<T>::Insert(UIntOS index, T Val)
	{
		if (this->objCnt == this->capacity)
		{
			UIntOS i;
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
			UIntOS j = this->objCnt;
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
