#ifndef _SM_DATA_ARRAYLISTBASE
#define _SM_DATA_ARRAYLISTBASE
#include "MemTool.h"
#include "MyMemory.h"
#include "Data/ArrayCollection.hpp"
#include "Data/List.hpp"

namespace Data
{
	template <class T> class ArrayListBase : public List<T>, public ArrayCollection<T>
	{
	protected:
		UnsafeArray<T> arr;
		UOSInt objCnt;
		UOSInt capacity;

		void Init(UOSInt capacity);
	protected:
		ArrayListBase();
		ArrayListBase(UOSInt capacity);
	public:
		virtual ~ArrayListBase();

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


	template <class T> void ArrayListBase<T>::Init(UOSInt capacity)
	{
		objCnt = 0;
		this->capacity = capacity;
		arr = MemAllocArr(T, capacity);
	}

	template <class T> ArrayListBase<T>::ArrayListBase()
	{
		Init(40);
	}

	template <class T> ArrayListBase<T>::ArrayListBase(UOSInt capacity)
	{
		Init(capacity);
	}

	template <class T> ArrayListBase<T>::~ArrayListBase()
	{
		MemFreeArr(arr);
	}

	template <class T> Bool ArrayListBase<T>::Remove(T val)
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

	template <class T> UOSInt ArrayListBase<T>::IndexOf(T val) const
	{
		UOSInt i = objCnt;
		while (i-- > 0)
			if (arr[i] == val)
				return i;
		return INVALID_INDEX;
	}

	template <class T> void ArrayListBase<T>::Clear()
	{
		this->objCnt = 0;
	}

	template <class T> UOSInt ArrayListBase<T>::GetCount() const
	{
		return this->objCnt;
	}

	template <class T> UOSInt ArrayListBase<T>::GetCapacity() const
	{
		return this->capacity;
	}

	template <class T> T ArrayListBase<T>::GetItem(UOSInt Index) const
	{
		if (Index >= this->objCnt || Index < 0)
			return (T)0;
		return this->arr[Index];
	}

	template <class T> void ArrayListBase<T>::SetItem(UOSInt Index, T Val)
	{
		if (Index == objCnt)
		{
			this->Add(Val);
		}
		else if (Index < objCnt)
		{
			this->arr[Index] = Val;
		}
		else
		{
			return;
		}
	}

	template <class T> UnsafeArray<T> ArrayListBase<T>::GetArr(OutParam<UOSInt> arraySize) const
	{
		arraySize.Set(this->objCnt);
		return this->arr;
	}

	template <class T> UnsafeArray<T> ArrayListBase<T>::Arr() const
	{
		return this->arr;
	}

	template <class T> T ArrayListBase<T>::Pop()
	{
		if (this->objCnt == 0) return 0;
		T o = arr[this->objCnt - 1];
		this->objCnt--;
		return o;
	}

	template <class T> void ArrayListBase<T>::Reverse()
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
#endif
