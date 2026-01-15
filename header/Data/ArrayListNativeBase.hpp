#ifndef _SM_DATA_ARRAYLISTNATIVEBASE
#define _SM_DATA_ARRAYLISTNATIVEBASE
#include "MemTool.h"
#include "MyMemory.h"
#include "Data/ArrayCollection.hpp"
#include "Data/ArrayListUtil.hpp"
#include "Data/List.hpp"

namespace Data
{
	template <class T> class ArrayListNativeBase : public List<T>, public ArrayCollection<T>
	{
	protected:
		UnsafeArray<T> arr;
		UIntOS objCnt;
		UIntOS capacity;

		void Init(UIntOS capacity);
	protected:
		ArrayListNativeBase();
		ArrayListNativeBase(UIntOS capacity);
	public:
		virtual ~ArrayListNativeBase();

		virtual Bool Remove(T val);
		virtual UIntOS IndexOf(T val) const;
		virtual void Clear();

		virtual UIntOS GetCount() const;
		virtual UIntOS GetCapacity() const;

		virtual T GetItem(UIntOS index) const;
		virtual void SetItem(UIntOS index, T val);
		void CopyItems(UIntOS destIndex, UIntOS srcIndex, UIntOS count);
		UIntOS GetRange(UnsafeArray<T> outArr, UIntOS index, UIntOS cnt) const;
		UIntOS RemoveRange(UIntOS index, UIntOS cnt);
		void AddAll(NN<const ArrayCollection<T>> arr);
		virtual UnsafeArray<T> GetArr(OutParam<UIntOS> arraySize) const;
		virtual UnsafeArray<T> Arr() const;
		T Pop();
		void Reverse();
	};


	template <class T> void ArrayListNativeBase<T>::Init(UIntOS capacity)
	{
		objCnt = 0;
		this->capacity = capacity;
		arr = MemAllocArr(T, capacity);
	}

	template <class T> ArrayListNativeBase<T>::ArrayListNativeBase()
	{
		Init(40);
	}

	template <class T> ArrayListNativeBase<T>::ArrayListNativeBase(UIntOS capacity)
	{
		Init(capacity);
	}

	template <class T> ArrayListNativeBase<T>::~ArrayListNativeBase()
	{
		MemFreeArr(arr);
	}

	template <class T> Bool ArrayListNativeBase<T>::Remove(T val)
	{
		UIntOS i = 0;
		UIntOS j = this->objCnt;
		UIntOS k = 0;
		while (i < j)
		{
			if (this->arr[i] == val)
			{

			}
			else if (i != k)
			{
				this->arr[k] = this->arr[i];
				k++;
			}
			else
			{
				k++;
			}
			i++;
		}
		if (i != k)
		{
			this->objCnt = k;
		}
		return i != k;
	}

	template <class T> UIntOS ArrayListNativeBase<T>::IndexOf(T val) const
	{
		UIntOS i = objCnt;
		while (i-- > 0)
			if (arr[i] == val)
				return i;
		return INVALID_INDEX;
	}

	template <class T> void ArrayListNativeBase<T>::Clear()
	{
		this->objCnt = 0;
	}

	template <class T> UIntOS ArrayListNativeBase<T>::GetCount() const
	{
		return this->objCnt;
	}

	template <class T> UIntOS ArrayListNativeBase<T>::GetCapacity() const
	{
		return this->capacity;
	}

	template <class T> T ArrayListNativeBase<T>::GetItem(UIntOS index) const
	{
		if (index >= this->objCnt)
			return (T)0;
		return this->arr[index];
	}

	template <class T> void ArrayListNativeBase<T>::SetItem(UIntOS index, T val)
	{
		if (index == objCnt)
		{
			this->Add(val);
		}
		else if (index < objCnt)
		{
			this->arr[index] = val;
		}
		else
		{
			return;
		}
	}

	template <class T> void ArrayListNativeBase<T>::AddAll(NN<const ArrayCollection<T>> arr)
	{
		UIntOS i = 0;
		UIntOS j = arr->GetCount();
		UnsafeArray<T> arr2 = arr->Arr();
		while (i < j)
		{
			this->Add(arr2[i]);
			i++;
		}
	}

	template <class T> UnsafeArray<T> ArrayListNativeBase<T>::GetArr(OutParam<UIntOS> arraySize) const
	{
		arraySize.Set(this->objCnt);
		return this->arr;
	}

	template <class T> UnsafeArray<T> ArrayListNativeBase<T>::Arr() const
	{
		return this->arr;
	}

	template <class T> T ArrayListNativeBase<T>::Pop()
	{
		if (this->objCnt == 0) return 0;
		T o = arr[this->objCnt - 1];
		this->objCnt--;
		return o;
	}

	template <class T> void ArrayListNativeBase<T>::Reverse()
	{
		T tmp;
		if (this->objCnt > 0)
		{
			UIntOS i = 0;
			UIntOS j = this->objCnt - 1;
			while (i < j)
			{
				tmp = this->arr[i];
				this->arr[i] = this->arr[j];
				this->arr[j] = tmp;
				i++;
				j--;
			}
		}
	}
}

#endif
