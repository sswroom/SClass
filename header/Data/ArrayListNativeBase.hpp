#ifndef _SM_DATA_ARRAYLISTNATIVEBASE
#define _SM_DATA_ARRAYLISTNATIVEBASE
#include "MemTool.h"
#include "MyMemory.h"
#include "Data/ArrayCollection.hpp"
#include "Data/List.hpp"

namespace Data
{
	template <class T> class ArrayListNativeBase : public List<T>, public ArrayCollection<T>
	{
	protected:
		UnsafeArray<T> arr;
		UOSInt objCnt;
		UOSInt capacity;

		void Init(UOSInt capacity);
	protected:
		ArrayListNativeBase();
		ArrayListNativeBase(UOSInt capacity);
	public:
		virtual ~ArrayListNativeBase();

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


	template <class T> void ArrayListNativeBase<T>::Init(UOSInt capacity)
	{
		objCnt = 0;
		this->capacity = capacity;
		arr = MemAllocArr(T, capacity);
	}

	template <class T> ArrayListNativeBase<T>::ArrayListNativeBase()
	{
		Init(40);
	}

	template <class T> ArrayListNativeBase<T>::ArrayListNativeBase(UOSInt capacity)
	{
		Init(capacity);
	}

	template <class T> ArrayListNativeBase<T>::~ArrayListNativeBase()
	{
		MemFreeArr(arr);
	}

	template <class T> Bool ArrayListNativeBase<T>::Remove(T val)
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

	template <class T> UOSInt ArrayListNativeBase<T>::IndexOf(T val) const
	{
		UOSInt i = objCnt;
		while (i-- > 0)
			if (arr[i] == val)
				return i;
		return INVALID_INDEX;
	}

	template <class T> void ArrayListNativeBase<T>::Clear()
	{
		this->objCnt = 0;
	}

	template <class T> UOSInt ArrayListNativeBase<T>::GetCount() const
	{
		return this->objCnt;
	}

	template <class T> UOSInt ArrayListNativeBase<T>::GetCapacity() const
	{
		return this->capacity;
	}

	template <class T> T ArrayListNativeBase<T>::GetItem(UOSInt index) const
	{
		if (index >= this->objCnt)
			return (T)0;
		return this->arr[index];
	}

	template <class T> void ArrayListNativeBase<T>::SetItem(UOSInt index, T val)
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

	template <class T> UnsafeArray<T> ArrayListNativeBase<T>::GetArr(OutParam<UOSInt> arraySize) const
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

#define LIST_CALL_FUNC(list, func) { UOSInt i = (list)->GetCount(); while (i-- > 0) func((list)->GetItem(i)); }
#define LIST_FREE_FUNC(list, func) { LIST_CALL_FUNC(list, func); (list)->Clear(); }
#define LIST_FREE_STRING(list) { UOSInt i = (list)->GetCount(); while (i-- > 0) (list)->GetItem(i)->Release(); (list)->Clear(); }
#define LIST_FREE_STRING_NO_CLEAR(list) { UOSInt i = (list)->GetCount(); while (i-- > 0) (list)->GetItem(i)->Release(); }
#define NNLIST_FREE_STRING(list) { UOSInt i = (list)->GetCount(); while (i-- > 0) (list)->GetItemNoCheck(i)->Release(); (list)->Clear(); }
#define NNLIST_CALL_FUNC(list, func) { UOSInt i = (list)->GetCount(); while (i-- > 0) func((list)->GetItemNoCheck(i)); }
#endif
