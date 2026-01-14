#ifndef _SM_DATA_ARRAYLISTOBJ
#define _SM_DATA_ARRAYLISTOBJ
#include "Data/ArrayListObjBase.hpp"

namespace Data
{
	// ArrayList for object types
	template <class T> class ArrayListObj : public ArrayListObjBase<T>
	{
	public:
		ArrayListObj();
		ArrayListObj(UOSInt capacity);
		ArrayListObj(const ArrayListObj<T> &list);
		virtual ~ArrayListObj();

		virtual UOSInt Add(T val);
		UOSInt AddAll(NN<const ReadingList<T>> arr);
		virtual UOSInt AddRange(UnsafeArray<const T> arr, UOSInt cnt);
		virtual T RemoveAt(UOSInt index);
		virtual void Insert(UOSInt index, T val);
		virtual NN<ArrayListObj<T>> Clone() const;

		void EnsureCapacity(UOSInt capacity);
		UOSInt GetRange(UnsafeArray<T> outArr, UOSInt index, UOSInt cnt) const;
		UOSInt RemoveRange(UOSInt index, UOSInt cnt);
		ArrayListObj<T> &operator =(const ArrayListObj<T> &v);
	};

	template <class T> ArrayListObj<T>::ArrayListObj() : ArrayListObjBase<T>()
	{
	}

	template <class T> ArrayListObj<T>::ArrayListObj(UOSInt capacity) : ArrayListObjBase<T>(capacity)
	{
	}

	template <class T> ArrayListObj<T>::ArrayListObj(const ArrayListObj<T> &list) : ArrayListObjBase<T>(list.capacity)
	{
		this->AddAll(list);
	}

	template <class T> ArrayListObj<T>::~ArrayListObj()
	{
	}

	template <class T> UOSInt ArrayListObj<T>::Add(T val)
	{
		UOSInt ret;
		if (this->objCnt == this->capacity)
		{
			UnsafeArray<T> newArr = MemAllocArr(T, this->capacity << 1);
			newArr.CopyFromNO(this->arr, this->objCnt);
			this->capacity = this->capacity << 1;
			MemFreeArr(this->arr);
			this->arr = newArr;
		}
		this->arr[ret = this->objCnt++] = val;
		return ret;
	}

	template <class T> UOSInt ArrayListObj<T>::AddAll(NN<const ReadingList<T>> arr)
	{
		UOSInt cnt = arr->GetCount();
		if (this->objCnt + cnt >= this->capacity)
		{
			while (this->objCnt + cnt >= this->capacity)
			{
				this->capacity = this->capacity << 1;
			}
			UnsafeArray<T> newArr = MemAllocArr(T, this->capacity);
			if (this->objCnt > 0)
			{
				newArr.CopyFromNO(this->arr, this->objCnt);
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

	template <class T> UOSInt ArrayListObj<T>::AddRange(UnsafeArray<const T> arr, UOSInt cnt)
	{
		if (this->objCnt + cnt >= this->capacity)
		{
			while (this->objCnt + cnt >= this->capacity)
			{
				this->capacity = this->capacity << 1;
			}
			UnsafeArray<T> newArr = MemAllocArr(T, this->capacity);
			if (this->objCnt > 0)
			{
				newArr.CopyFromNO(this->arr, this->objCnt);
			}
			MemFreeArr(this->arr);
			this->arr = newArr;
		}
		MemCopyNO(&this->arr[this->objCnt], arr.Ptr(), cnt * sizeof(T));
		this->objCnt += cnt;
		return cnt;
	}

	template <class T> T ArrayListObj<T>::RemoveAt(UOSInt index)
	{
		if (index >= this->objCnt)
			return nullptr;
		UOSInt i = this->objCnt - index - 1;
		T o = this->arr[index];
		if (i > 0)
		{
			MemCopyO(&this->arr[index], &this->arr[index + 1], i * sizeof(T));
		}
		this->objCnt--;
		//arr[objCnt] = (T)0;
		return o;
	}

	template <class T> void ArrayListObj<T>::Insert(UOSInt index, T Val)
	{
		if (this->objCnt == this->capacity)
		{
			UnsafeArray<T> newArr = MemAllocArr(T, this->capacity << 1);
			if (index > 0)
			{
				newArr.CopyFromNO(this->arr, index);
			}
			newArr[index] = Val;
			if (index < this->objCnt)
			{
				MemCopyNO(&newArr[index + 1], &this->arr[index], (this->objCnt - index) * sizeof(T));
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

	template <class T> NN<ArrayListObj<T>> ArrayListObj<T>::Clone() const
	{
		NN<ArrayListObj<T>> newArr;
		NEW_CLASSNN(newArr, ArrayListObj<T>(this->capacity));
		newArr->AddAll(NNTHIS);
		return newArr;
	}

	template <class T> void ArrayListObj<T>::EnsureCapacity(UOSInt capacity)
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

	template <class T> UOSInt ArrayListObj<T>::GetRange(UnsafeArray<T> outArr, UOSInt index, UOSInt cnt) const
	{
		UOSInt startIndex = index;
		UOSInt endIndex = index + cnt;
		if (endIndex > this->objCnt)
		{
			endIndex = this->objCnt;
		}
		if (startIndex >= this->objCnt)
			return 0;
		if (endIndex <= startIndex)
		{
			return 0;
		}
		MemCopyNO(outArr.Ptr(), &this->arr[startIndex], sizeof(T) * (endIndex - startIndex));
		return endIndex - startIndex;
	}

	template <class T> UOSInt ArrayListObj<T>::RemoveRange(UOSInt index, UOSInt cnt)
	{
		UOSInt startIndex = index;
		UOSInt endIndex = index + cnt;
		if (endIndex > this->objCnt)
		{
			endIndex = this->objCnt;
		}
		if (startIndex >= this->objCnt)
			return 0;
		if (endIndex <= startIndex)
		{
			return 0;
		}
#if defined(_MSC_VER)
		MemCopyO(&this->arr[startIndex], &this->arr[endIndex], sizeof(T) * (this->objCnt - endIndex));
#else
		UOSInt i = startIndex;
		UOSInt j = endIndex;
		while (j < this->objCnt)
		{
			this->arr[i++] = this->arr[j++];
		}
#endif
		this->objCnt -= endIndex - startIndex;
		return endIndex - startIndex;
	}

	template <class T> ArrayListObj<T> &ArrayListObj<T>::operator =(const ArrayListObj<T> &v)
	{
		this->Clear();
		this->EnsureCapacity(v.capacity);
		this->AddAll(v);
		return NNTHIS;
	}

}
#endif
