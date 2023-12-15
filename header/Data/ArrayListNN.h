#ifndef _SM_DATA_ARRAYLISTNN
#define _SM_DATA_ARRAYLISTNN
#include "MemTool.h"
#include "MyMemory.h"
#include "Data/ArrayCollection.h"
#include "Data/ReadingList.h"

namespace Data
{
	template <class T> class ArrayListNN : public ReadingList<Optional<T>>, public ArrayCollection<NotNullPtr<T>>
	{
	public:
		typedef void (*FreeFunc)(NotNullPtr<T> v);
	protected:
		NotNullPtr<T> *arr;
		UOSInt objCnt;
		UOSInt capacity;

		void Init(UOSInt capacity);
	public:
		ArrayListNN();
		ArrayListNN(UOSInt capacity);
		ArrayListNN(const ArrayListNN<T> &list);
		virtual ~ArrayListNN();

		virtual UOSInt Add(NotNullPtr<T> val);
		virtual UOSInt AddRange(const NotNullPtr<T> *arr, UOSInt cnt);
		UOSInt AddAll(NotNullPtr<const ArrayListNN<T>> list);
		virtual Bool Remove(NotNullPtr<T> val);
		virtual Optional<T> RemoveAt(UOSInt index);
		virtual void Insert(UOSInt index, NotNullPtr<T> val);
		virtual UOSInt IndexOf(NotNullPtr<T> val) const;
		virtual void Clear();
		virtual NotNullPtr<ArrayListNN<T>> Clone() const;

		virtual UOSInt GetCount() const;
		virtual UOSInt GetCapacity() const;
		void EnsureCapacity(UOSInt capacity);

		virtual Optional<T> GetItem(UOSInt index) const;
		virtual Optional<T> SetItem(UOSInt index, NotNullPtr<T> val);
		void CopyItems(UOSInt destIndex, UOSInt srcIndex, UOSInt count);
		UOSInt GetRange(NotNullPtr<T> *outArr, UOSInt index, UOSInt cnt) const;
		UOSInt RemoveRange(UOSInt index, UOSInt cnt);
		virtual NotNullPtr<T> *GetPtr(OutParam<UOSInt> arraySize) const;
		virtual NotNullPtr<T> *Ptr() const;
		Optional<T> GetLast();
		Optional<T> Pop();
		ArrayListNN<T> &operator =(const ArrayListNN<T> &v);
		void DeleteAll();
		void FreeAll(FreeFunc freeFunc);
	};


	template <class T> void ArrayListNN<T>::Init(UOSInt capacity)
	{
		this->objCnt = 0;
		this->capacity = capacity;
		this->arr = MemAlloc(NotNullPtr<T>, capacity);
	}

	template <class T> ArrayListNN<T>::ArrayListNN()
	{
		Init(40);
	}

	template <class T> ArrayListNN<T>::ArrayListNN(UOSInt capacity)
	{
		Init(capacity);
	}

	template <class T> ArrayListNN<T>::ArrayListNN(const ArrayListNN<T> &list)
	{
		Init(list.capacity);
		this->AddAll(list);
	}

	template <class T> ArrayListNN<T>::~ArrayListNN()
	{
		MemFree(arr);
		arr = 0;
	}

	template <class T> UOSInt ArrayListNN<T>::Add(NotNullPtr<T> val)
	{
		UOSInt ret;
		if (objCnt >= this->capacity)
		{
			this->capacity = this->capacity << 1;
			NotNullPtr<T> *newArr = MemAlloc(NotNullPtr<T>, this->capacity);
			MemCopyNO(newArr, arr, this->objCnt * sizeof(NotNullPtr<T>));
			MemFree(arr);
			arr = newArr;
		}
		arr[ret = objCnt++] = val;
		return ret;
	}

	template <class T> UOSInt ArrayListNN<T>::AddAll(NotNullPtr<const ArrayListNN<T>> arr)
	{
		UOSInt cnt = arr->GetCount();
		if (objCnt + cnt >= this->capacity)
		{
			while (objCnt + cnt >= this->capacity)
			{
				this->capacity = this->capacity << 1;
			}
			NotNullPtr<T> *newArr = MemAlloc(NotNullPtr<T>, this->capacity);
			if (objCnt > 0)
			{
				MemCopyNO(newArr, this->arr, objCnt * sizeof(NotNullPtr<T>));
			}
			MemFree(this->arr);
			this->arr = newArr;
		}
		UOSInt i = 0;
		while (i < cnt)
		{
			this->arr[this->objCnt + i] = arr->arr[i];
			i++;
		}
		this->objCnt += cnt;
		return cnt;
	}

	template <class T> UOSInt ArrayListNN<T>::AddRange(const NotNullPtr<T> *arr, UOSInt cnt)
	{
		if (objCnt + cnt >= this->capacity)
		{
			while (objCnt + cnt >= this->capacity)
			{
				this->capacity = this->capacity << 1;
			}
			NotNullPtr<T> *newArr = MemAlloc(NotNullPtr<T>, this->capacity);
			if (objCnt > 0)
			{
				MemCopyNO(newArr, this->arr, objCnt * sizeof(NotNullPtr<T>));
			}
			MemFree(this->arr);
			this->arr = newArr;
		}
		MemCopyNO(&this->arr[objCnt], arr, cnt * sizeof(NotNullPtr<T>));
		this->objCnt += cnt;
		return cnt;
	}

	template <class T> Bool ArrayListNN<T>::Remove(NotNullPtr<T> val)
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

	template <class T> Optional<T> ArrayListNN<T>::RemoveAt(UOSInt index)
	{
		if (index >= this->objCnt)
			return nullptr;
		UOSInt i = this->objCnt - index - 1;
		NotNullPtr<T> o = arr[index];
		if (i > 0)
		{
			MemCopyO(&arr[index], &arr[index + 1], i * sizeof(NotNullPtr<T>));
		}
		this->objCnt--;
		//arr[objCnt] = (T)0;
		return o;
	}

	template <class T> void ArrayListNN<T>::Insert(UOSInt Index, NotNullPtr<T> Val)
	{
		if (objCnt == this->capacity)
		{
			NotNullPtr<T> *newArr = MemAlloc(NotNullPtr<T>, this->capacity * 2);
			if (Index > 0)
			{
				MemCopyNO(newArr, this->arr, Index * sizeof(NotNullPtr<T>));
			}
			newArr[Index] = Val;
			if (Index < this->objCnt)
			{
				MemCopyNO(&newArr[Index + 1], &this->arr[Index], (this->objCnt - Index) * sizeof(NotNullPtr<T>));
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

	template <class T> UOSInt ArrayListNN<T>::IndexOf(NotNullPtr<T> val) const
	{
		UOSInt i = objCnt;
		while (i-- > 0)
			if (arr[i] == val)
				return i;
		return INVALID_INDEX;
	}

	template <class T> void ArrayListNN<T>::Clear()
	{
		this->objCnt = 0;
	}

	template <class T> NotNullPtr<ArrayListNN<T>> ArrayListNN<T>::Clone() const
	{
		NotNullPtr<ArrayListNN<T>> newArr;
		NEW_CLASSNN(newArr, ArrayListNN<T>(this->capacity));
		newArr->AddAll(*this);
		return newArr;
	}

	template <class T> UOSInt ArrayListNN<T>::GetCount() const
	{
		return this->objCnt;
	}

	template <class T> UOSInt ArrayListNN<T>::GetCapacity() const
	{
		return this->capacity;
	}

	template <class T> void ArrayListNN<T>::EnsureCapacity(UOSInt capacity)
	{
		if (capacity > this->capacity)
		{
			while (capacity > this->capacity)
			{
				this->capacity = this->capacity << 1;
			}

			NotNullPtr<T> *newArr = MemAlloc(NotNullPtr<T>, this->capacity);
			MemCopyNO(newArr, this->arr, this->objCnt * sizeof(NotNullPtr<T>));
			MemFree(this->arr);
			this->arr = newArr;
		}
	}

	template <class T> Optional<T> ArrayListNN<T>::GetItem(UOSInt index) const
	{
		if (index >= this->objCnt || index < 0)
			return nullptr;
		return this->arr[index];
	}

	template <class T> Optional<T> ArrayListNN<T>::SetItem(UOSInt index, NotNullPtr<T> val)
	{
		if (index == objCnt)
		{
			Add(val);
			return Optional<T>(nullptr);
		}
		else if (index < objCnt)
		{
			NotNullPtr<T> oriVal = this->arr[index];
			this->arr[index] = val;
			return oriVal;
		}
		else
		{
			return Optional<T>(nullptr);
		}
	}

	template <class T> void ArrayListNN<T>::CopyItems(UOSInt destIndex, UOSInt srcIndex, UOSInt count)
	{
		MemCopyO(&this->arr[destIndex], &this->arr[srcIndex], count * sizeof(this->arr[0]));
	}

	template <class T> UOSInt ArrayListNN<T>::GetRange(NotNullPtr<T> *outArr, UOSInt Index, UOSInt cnt) const
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
		MemCopyNO(outArr, &arr[startIndex], sizeof(NotNullPtr<T>) * (endIndex - startIndex));
		return endIndex - startIndex;
	}

	template <class T> UOSInt ArrayListNN<T>::RemoveRange(UOSInt Index, UOSInt cnt)
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
		MemCopyO(&arr[startIndex], &arr[endIndex], sizeof(NotNullPtr<T>) * (objCnt - endIndex));
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

	template <class T> NotNullPtr<T>* ArrayListNN<T>::GetPtr(OutParam<UOSInt> arraySize) const
	{
		arraySize.Set(this->objCnt);
		return this->arr;
	}
	
	template <class T> NotNullPtr<T>* ArrayListNN<T>::Ptr() const
	{
		return this->arr;
	}

	template <class T> Optional<T> ArrayListNN<T>::GetLast()
	{
		if (this->objCnt == 0) return 0;
		return arr[this->objCnt - 1];
	}

	template <class T> Optional<T> ArrayListNN<T>::Pop()
	{
		if (this->objCnt == 0) return 0;
		NotNullPtr<T> o = arr[this->objCnt - 1];
		this->objCnt--;
		return o;
	}

	template <class T> ArrayListNN<T> &ArrayListNN<T>::operator =(const ArrayListNN<T> &v)
	{
		this->Clear();
		this->EnsureCapacity(v.capacity);
		this->AddAll(&v);
		return *this;
	}

	template <class T> void ArrayListNN<T>::DeleteAll()
	{
		UOSInt i = this->objCnt;
		while (i-- > 0)
		{
			this->arr[i].Delete();
		}
		this->objCnt = 0;
	}

	template <class T> void ArrayListNN<T>::FreeAll(FreeFunc freeFunc)
	{
		UOSInt i = this->objCnt;
		while (i-- > 0)
		{
			freeFunc(this->arr[i]);
		}
		this->objCnt = 0;
	}
}

#define LISTNN_FREE_STRING(list) { Data::ArrayIterator<NotNullPtr<Text::String>> it = (list)->Iterator(); while (it.HasNext()) it.Next()->Release(); (list)->Clear(); }
#endif
