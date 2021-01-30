#ifndef _SM_DATA_ARRAYLIST
#define _SM_DATA_ARRAYLIST
#include "MyMemory.h"
#include "Data/ArrayCollection.h"
#include "Data/List.h"

namespace Data
{
	template <class T> class ArrayList : public List<T>, public ArrayCollection<T>
	{
	protected:
		T* arr;
		UOSInt objCnt;
		UOSInt capacity;

		void Init(UOSInt Capacity);
	public:
		ArrayList();
		ArrayList(UOSInt Capacity);
		virtual ~ArrayList();

		virtual UOSInt Add(T val);
		UOSInt AddRange(ArrayList<T> *arr);
		UOSInt AddRange(T *arr, UOSInt cnt);
		Bool Remove(T val);
		T RemoveAt(UOSInt index);
		void Insert(UOSInt Index, T Val);
		UOSInt IndexOf(T Val);
		void Clear();
		virtual ArrayList<T> *Clone();

		virtual UOSInt GetCount();
		UOSInt GetCapacity();
		void EnsureCapacity(UOSInt Capacity);

		virtual T GetItem(UOSInt Index);
		void SetItem(UOSInt Index, T Val);
		void CopyItems(UOSInt destIndex, UOSInt srcIndex, UOSInt count);
		UOSInt GetRange(T *outArr, UOSInt Index, UOSInt cnt);
		UOSInt RemoveRange(UOSInt Index, UOSInt cnt);
		virtual T *GetArray(UOSInt *arraySize);
	};


	template <class T> void ArrayList<T>::Init(UOSInt Capacity)
	{
		objCnt = 0;
		this->capacity = Capacity;
		arr = MemAlloc(T, Capacity);
	}

	template <class T> ArrayList<T>::ArrayList()
	{
		Init(40);
	}

	template <class T> ArrayList<T>::ArrayList(UOSInt Capacity)
	{
		Init(Capacity);
	}

	template <class T> ArrayList<T>::~ArrayList()
	{
		MemFree(arr);
		arr = 0;
	}

	template <class T> UOSInt ArrayList<T>::Add(T val)
	{
		UOSInt ret;
		if (objCnt == this->capacity)
		{
			T *newArr = MemAlloc(T, this->capacity * 2);
			MemCopyNO(newArr, arr, this->objCnt * sizeof(T));
			this->capacity = this->capacity << 1;
			MemFree(arr);
			arr = newArr;
		}
		arr[ret = objCnt++] = val;
		return ret;
	}

	template <class T> UOSInt ArrayList<T>::AddRange(ArrayList<T> *arr)
	{
		UOSInt cnt = arr->GetCount();
		if (objCnt + cnt >= this->capacity)
		{
			while (objCnt + cnt >= this->capacity)
			{
				this->capacity = this->capacity << 1;
			}
			T *newArr = MemAlloc(T, this->capacity);
			if (objCnt > 0)
			{
				MemCopyNO(newArr, this->arr, objCnt * sizeof(T));
			}
			MemFree(this->arr);
			this->arr = newArr;
		}
		MemCopyNO(&this->arr[objCnt], arr->arr, cnt * sizeof(T));
		this->objCnt += cnt;
		return cnt;
	}

	template <class T> UOSInt ArrayList<T>::AddRange(T *arr, UOSInt cnt)
	{
		if (objCnt + cnt >= this->capacity)
		{
			while (objCnt + cnt >= this->capacity)
			{
				this->capacity = this->capacity << 1;
			}
			T *newArr = MemAlloc(T, this->capacity);
			if (objCnt > 0)
			{
				MemCopyNO(newArr, this->arr, objCnt * sizeof(T));
			}
			MemFree(this->arr);
			this->arr = newArr;
		}
		MemCopyNO(&this->arr[objCnt], arr, cnt * sizeof(T));
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
#if defined(_MSC_VER)
			MemCopyO(&arr[index], &arr[index + 1], i * sizeof(T));
#else
			while (i-- > 0)
			{
				arr[index] = arr[index + 1];
				index++;
			}
#endif
		}
		this->objCnt--;
		//arr[objCnt] = (T)0;
		return o;
	}

	template <class T> void ArrayList<T>::Insert(UOSInt Index, T Val)
	{
		if (objCnt == this->capacity)
		{
			T *newArr = MemAlloc(T, this->capacity * 2);
			if (Index > 0)
			{
				MemCopyNO(newArr, this->arr, Index * sizeof(T));
			}
			newArr[Index] = Val;
			if (Index < this->objCnt)
			{
				MemCopyNO(&newArr[Index + 1], &this->arr[Index], (this->objCnt - Index) * sizeof(T));
			}
			this->capacity = this->capacity << 1;
			MemFree(arr);
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

	template <class T> UOSInt ArrayList<T>::IndexOf(T Val)
	{
		UOSInt i = objCnt;
		while (i-- > 0)
			if (arr[i] == Val)
				return i;
		return -1;
	}

	template <class T> void ArrayList<T>::Clear()
	{
		UOSInt i = objCnt;
		while (i-- > 0)
		{
			arr[i] = 0;
		}
		this->objCnt = 0;
	}

	template <class T> ArrayList<T> *ArrayList<T>::Clone()
	{
		ArrayList<T> *newArr = new ArrayList<T>(this->capacity);
		newArr->AddRange(this);
		return newArr;
	}

	template <class T> UOSInt ArrayList<T>::GetCount()
	{
		return this->objCnt;
	}

	template <class T> UOSInt ArrayList<T>::GetCapacity()
	{
		return this->capacity;
	}

	template <class T> void ArrayList<T>::EnsureCapacity(UOSInt Capacity)
	{
		if (Capacity > this->capacity)
		{
			while (Capacity > this->capacity)
			{
				this->capacity = this->capacity << 1;
			}

			T *newArr = MemAlloc(T, this->capacity);
			MemCopyNO(newArr, this->arr, this->objCnt * sizeof(T));
			MemFree(this->arr);
			this->arr = newArr;
		}
	}

	template <class T> T ArrayList<T>::GetItem(UOSInt Index)
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

	template <class T> UOSInt ArrayList<T>::GetRange(T *outArr, UOSInt Index, UOSInt cnt)
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
		MemCopyNO(outArr, &arr[startIndex], sizeof(T) * (endIndex - startIndex));
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

	template <class T> T* ArrayList<T>::GetArray(UOSInt *arraySize)
	{
		*arraySize = this->objCnt;
		return this->arr;
	}
}

#define DEL_LIST_FUNC(list, func) { UOSInt i = list->GetCount(); while (i-- > 0) func(list->GetItem(i)); }

#endif
