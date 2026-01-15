#ifndef _SM_DATA_ARRAYLISTOBJBASE
#define _SM_DATA_ARRAYLISTOBJBASE
#include "MemTool.h"
#include "MyMemory.h"
#include "Data/ArrayCollection.hpp"
#include "Data/ArrayListUtil.hpp"
#include "Data/List.hpp"

namespace Data
{
	template <class T> class ArrayListObjBase : public List<T>, public ArrayCollection<T>
	{
	protected:
		UnsafeArray<T> arr;
		UOSInt objCnt;
		UOSInt capacity;

		void Init(UOSInt capacity);
	protected:
		ArrayListObjBase();
		ArrayListObjBase(UOSInt capacity);
	public:
		virtual ~ArrayListObjBase();

		virtual Bool Remove(T val);
		virtual UOSInt IndexOf(T val) const;
		virtual void Clear();

		virtual UOSInt GetCount() const;
		virtual UOSInt GetCapacity() const;

		virtual T GetItem(UOSInt index) const;
		virtual void SetItem(UOSInt index, T val);
		void CopyItems(UOSInt destIndex, UOSInt srcIndex, UOSInt count);
		UOSInt GetRange(UnsafeArray<T> outArr, UOSInt index, UOSInt cnt) const;
		UOSInt RemoveRange(UOSInt index, UOSInt cnt);
		virtual UnsafeArray<T> GetArr(OutParam<UOSInt> arraySize) const;
		virtual UnsafeArray<T> Arr() const;
		T Pop();
		void Reverse();
	};


	template <class T> void ArrayListObjBase<T>::Init(UOSInt capacity)
	{
		objCnt = 0;
		this->capacity = capacity;
		arr = MemAllocArr(T, capacity);
	}

	template <class T> ArrayListObjBase<T>::ArrayListObjBase()
	{
		Init(40);
	}

	template <class T> ArrayListObjBase<T>::ArrayListObjBase(UOSInt capacity)
	{
		Init(capacity);
	}

	template <class T> ArrayListObjBase<T>::~ArrayListObjBase()
	{
		MemFreeArr(arr);
	}

	template <class T> Bool ArrayListObjBase<T>::Remove(T val)
	{
		UOSInt i = 0;
		UOSInt j = this->objCnt;
		UOSInt k = 0;
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

	template <class T> UOSInt ArrayListObjBase<T>::IndexOf(T val) const
	{
		UOSInt i = objCnt;
		while (i-- > 0)
			if (arr[i] == val)
				return i;
		return INVALID_INDEX;
	}

	template <class T> void ArrayListObjBase<T>::Clear()
	{
		this->objCnt = 0;
	}

	template <class T> UOSInt ArrayListObjBase<T>::GetCount() const
	{
		return this->objCnt;
	}

	template <class T> UOSInt ArrayListObjBase<T>::GetCapacity() const
	{
		return this->capacity;
	}

	template <class T> T ArrayListObjBase<T>::GetItem(UOSInt index) const
	{
		if (index >= this->objCnt)
			return nullptr;
		return this->arr[index];
	}

	template <class T> void ArrayListObjBase<T>::SetItem(UOSInt index, T val)
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

	template <class T> UnsafeArray<T> ArrayListObjBase<T>::GetArr(OutParam<UOSInt> arraySize) const
	{
		arraySize.Set(this->objCnt);
		return this->arr;
	}

	template <class T> UnsafeArray<T> ArrayListObjBase<T>::Arr() const
	{
		return this->arr;
	}

	template <class T> T ArrayListObjBase<T>::Pop()
	{
		if (this->objCnt == 0) return 0;
		T o = arr[this->objCnt - 1];
		this->objCnt--;
		return o;
	}

	template <class T> void ArrayListObjBase<T>::Reverse()
	{
		T tmp;
		if (this->objCnt > 0)
		{
			UOSInt i = 0;
			UOSInt j = this->objCnt - 1;
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
