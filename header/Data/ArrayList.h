#ifndef _SM_DATA_ARRAYLIST
#define _SM_DATA_ARRAYLIST
#include "MemTool.h"
#include "MyMemory.h"
#include "Data/ArrayCollection.h"
#include "Data/List.h"

namespace Data
{
	template <class T> class ArrayList : public List<T>, public ArrayCollection<T>
	{
	protected:
		UnsafeArray<T> arr;
		UOSInt objCnt;
		UOSInt capacity;

		void Init(UOSInt capacity);
	public:
		ArrayList();
		ArrayList(UOSInt capacity);
		ArrayList(const ArrayList<T> &list);
		virtual ~ArrayList();

		virtual UOSInt Add(T val);
		UOSInt AddAll(NN<const ReadingList<T>> arr);
		virtual UOSInt AddRange(UnsafeArray<const T> arr, UOSInt cnt);
		virtual Bool Remove(T val);
		virtual T RemoveAt(UOSInt index);
		virtual void Insert(UOSInt index, T val);
		virtual UOSInt IndexOf(T val) const;
		virtual void Clear();
		virtual NN<ArrayList<T>> Clone() const;

		virtual UOSInt GetCount() const;
		virtual UOSInt GetCapacity() const;
		void EnsureCapacity(UOSInt capacity);

		virtual T GetItem(UOSInt index) const;
		virtual void SetItem(UOSInt index, T val);
		void CopyItems(UOSInt destIndex, UOSInt srcIndex, UOSInt count);
		UOSInt GetRange(UnsafeArray<T> outArr, UOSInt index, UOSInt cnt) const;
		UOSInt RemoveRange(UOSInt index, UOSInt cnt);
		virtual UnsafeArray<T> GetArr(OutParam<UOSInt> arraySize) const;
		virtual UnsafeArray<T> Arr() const;
		T Pop();
		ArrayList<T> &operator =(const ArrayList<T> &v);
	};


	template <class T> void ArrayList<T>::Init(UOSInt capacity)
	{
		objCnt = 0;
		this->capacity = capacity;
		arr = MemAllocArr(T, capacity);
	}

	template <class T> ArrayList<T>::ArrayList()
	{
		Init(40);
	}

	template <class T> ArrayList<T>::ArrayList(UOSInt capacity)
	{
		Init(capacity);
	}

	template <class T> ArrayList<T>::ArrayList(const ArrayList<T> &list)
	{
		Init(list.capacity);
		this->AddAll(list);
	}

	template <class T> ArrayList<T>::~ArrayList()
	{
		MemFreeArr(arr);
	}

	template <class T> UOSInt ArrayList<T>::Add(T val)
	{
		UOSInt ret;
		if (objCnt == this->capacity)
		{
			UnsafeArray<T> newArr = MemAllocArr(T, this->capacity * 2);
			newArr.CopyFromNO(this->arr, this->objCnt);
			this->capacity = this->capacity << 1;
			MemFreeArr(arr);
			arr = newArr;
		}
		arr[ret = objCnt++] = val;
		return ret;
	}

	template <class T> UOSInt ArrayList<T>::AddAll(NN<const ReadingList<T>> arr)
	{
		UOSInt cnt = arr->GetCount();
		if (objCnt + cnt >= this->capacity)
		{
			while (objCnt + cnt >= this->capacity)
			{
				this->capacity = this->capacity << 1;
			}
			UnsafeArray<T> newArr = MemAllocArr(T, this->capacity);
			if (objCnt > 0)
			{
				newArr.CopyFromNO(this->arr, objCnt);
			}
			MemFreeArr(this->arr);
			this->arr = newArr;
		}
		UOSInt i = 0;
		while (i < cnt)
		{
			this->arr[this->objCnt + i] = arr->GetItem(i);
			i++;
		}
		this->objCnt += cnt;
		return cnt;
	}

	template <class T> UOSInt ArrayList<T>::AddRange(UnsafeArray<const T> arr, UOSInt cnt)
	{
		if (objCnt + cnt >= this->capacity)
		{
			while (objCnt + cnt >= this->capacity)
			{
				this->capacity = this->capacity << 1;
			}
			UnsafeArray<T> newArr = MemAllocArr(T, this->capacity);
			if (objCnt > 0)
			{
				newArr.CopyFromNO(this->arr, objCnt);
			}
			MemFreeArr(this->arr);
			this->arr = newArr;
		}
		MemCopyNO(&this->arr[objCnt], arr.Ptr(), cnt * sizeof(T));
		this->objCnt += cnt;
		return cnt;
	}

	template <class T> Bool ArrayList<T>::Remove(T val)
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

	template <class T> T ArrayList<T>::RemoveAt(UOSInt index)
	{
		if (index >= this->objCnt)
			return (T)0;
		UOSInt i = this->objCnt - index - 1;
		T o = arr[index];
		if (i > 0)
		{
			MemCopyO(&arr[index], &arr[index + 1], i * sizeof(T));
		}
		this->objCnt--;
		//arr[objCnt] = (T)0;
		return o;
	}

	template <class T> void ArrayList<T>::Insert(UOSInt Index, T Val)
	{
		if (objCnt == this->capacity)
		{
			UnsafeArray<T> newArr = MemAllocArr(T, this->capacity * 2);
			if (Index > 0)
			{
				newArr.CopyFromNO(this->arr, Index);
			}
			newArr[Index] = Val;
			if (Index < this->objCnt)
			{
				MemCopyNO(&newArr[Index + 1], &this->arr[Index], (this->objCnt - Index) * sizeof(T));
			}
			this->capacity = this->capacity << 1;
			MemFreeArr(arr);
			arr = newArr;
		}
		else
		{
			UOSInt j = this->objCnt;
			while (j > Index)
			{
				this->arr[j] = this->arr[j - 1];
				j--;
			}
			arr[Index] = Val;
		}
		objCnt++;
	}

	template <class T> UOSInt ArrayList<T>::IndexOf(T val) const
	{
		UOSInt i = objCnt;
		while (i-- > 0)
			if (arr[i] == val)
				return i;
		return INVALID_INDEX;
	}

	template <class T> void ArrayList<T>::Clear()
	{
		this->objCnt = 0;
	}

	template <class T> NN<ArrayList<T>> ArrayList<T>::Clone() const
	{
		NN<ArrayList<T>> newArr;
		NEW_CLASSNN(newArr, ArrayList<T>(this->capacity));
		newArr->AddAll(*this);
		return newArr;
	}

	template <class T> UOSInt ArrayList<T>::GetCount() const
	{
		return this->objCnt;
	}

	template <class T> UOSInt ArrayList<T>::GetCapacity() const
	{
		return this->capacity;
	}

	template <class T> void ArrayList<T>::EnsureCapacity(UOSInt capacity)
	{
		if (capacity > this->capacity)
		{
			while (capacity > this->capacity)
			{
				this->capacity = this->capacity << 1;
			}

			UnsafeArray<T> newArr = MemAllocArr(T, this->capacity);
			newArr.CopyFromNO(this->arr, this->objCnt);
			MemFreeArr(this->arr);
			this->arr = newArr;
		}
	}

	template <class T> T ArrayList<T>::GetItem(UOSInt Index) const
	{
		if (Index >= this->objCnt || Index < 0)
			return (T)0;
		return this->arr[Index];
	}

	template <class T> void ArrayList<T>::SetItem(UOSInt Index, T Val)
	{
		if (Index == objCnt)
		{
			Add(Val);
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

	template <class T> void ArrayList<T>::CopyItems(UOSInt destIndex, UOSInt srcIndex, UOSInt count)
	{
		MemCopyO(&this->arr[destIndex], &this->arr[srcIndex], count * sizeof(this->arr[0]));
	}

	template <class T> UOSInt ArrayList<T>::GetRange(UnsafeArray<T> outArr, UOSInt Index, UOSInt cnt) const
	{
		UOSInt startIndex = Index;
		UOSInt endIndex = Index + cnt;
		if (endIndex > objCnt)
		{
			endIndex = objCnt;
		}
		if (startIndex >= objCnt)
			return 0;
		if (endIndex <= startIndex)
		{
			return 0;
		}
		MemCopyNO(outArr.Ptr(), &arr[startIndex], sizeof(T) * (endIndex - startIndex));
		return endIndex - startIndex;
	}

	template <class T> UOSInt ArrayList<T>::RemoveRange(UOSInt Index, UOSInt cnt)
	{
		UOSInt startIndex = Index;
		UOSInt endIndex = Index + cnt;
		if (endIndex > objCnt)
		{
			endIndex = objCnt;
		}
		if (startIndex >= objCnt)
			return 0;
		if (endIndex <= startIndex)
		{
			return 0;
		}
#if defined(_MSC_VER)
		MemCopyO(&arr[startIndex], &arr[endIndex], sizeof(T) * (objCnt - endIndex));
#else
		UOSInt i = startIndex;
		UOSInt j = endIndex;
		while (j < objCnt)
		{
			arr[i++] = arr[j++];
		}
#endif
		objCnt -= endIndex - startIndex;
		return endIndex - startIndex;
	}

	template <class T> UnsafeArray<T> ArrayList<T>::GetArr(OutParam<UOSInt> arraySize) const
	{
		arraySize.Set(this->objCnt);
		return this->arr;
	}

	template <class T> UnsafeArray<T> ArrayList<T>::Arr() const
	{
		return this->arr;
	}

	template <class T> T ArrayList<T>::Pop()
	{
		if (this->objCnt == 0) return 0;
		T o = arr[this->objCnt - 1];
		this->objCnt--;
		return o;
	}

	template <class T> ArrayList<T> &ArrayList<T>::operator =(const ArrayList<T> &v)
	{
		this->Clear();
		this->EnsureCapacity(v.capacity);
		this->AddAll(v);
		return *this;
	}
}

#define LIST_CALL_FUNC(list, func) { UOSInt i = (list)->GetCount(); while (i-- > 0) func((list)->GetItem(i)); }
#define LIST_FREE_FUNC(list, func) { LIST_CALL_FUNC(list, func); (list)->Clear(); }
#define LIST_FREE_STRING(list) { UOSInt i = (list)->GetCount(); while (i-- > 0) (list)->GetItem(i)->Release(); (list)->Clear(); }
#define LIST_FREE_STRING_NO_CLEAR(list) { UOSInt i = (list)->GetCount(); while (i-- > 0) (list)->GetItem(i)->Release(); }
#define NNLIST_FREE_STRING(list) { UOSInt i = (list)->GetCount(); while (i-- > 0) (list)->GetItemNoCheck(i)->Release(); (list)->Clear(); }
#endif
