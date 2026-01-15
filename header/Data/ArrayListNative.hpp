#ifndef _SM_DATA_ARRAYLISTNATIVE
#define _SM_DATA_ARRAYLISTNATIVE
#include "Data/ArrayListNativeBase.hpp"

namespace Data
{
	// ArrayList for native types
	template <class T> class ArrayListNative : public ArrayListNativeBase<T>
	{
	public:
		ArrayListNative();
		ArrayListNative(UIntOS capacity);
		ArrayListNative(const ArrayListNative<T> &list);
		virtual ~ArrayListNative();

		virtual UIntOS Add(T val);
		UIntOS AddAll(NN<const ReadingList<T>> arr);
		virtual UIntOS AddRange(UnsafeArray<const T> arr, UIntOS cnt);
		virtual T RemoveAt(UIntOS index);
		virtual void Insert(UIntOS index, T val);
		virtual NN<ArrayListNative<T>> Clone() const;

		void EnsureCapacity(UIntOS capacity);
		UIntOS GetRange(UnsafeArray<T> outArr, UIntOS index, UIntOS cnt) const;
		UIntOS RemoveRange(UIntOS index, UIntOS cnt);
		ArrayListNative<T> &operator =(const ArrayListNative<T> &v);
	};

	template <class T> ArrayListNative<T>::ArrayListNative() : ArrayListNativeBase<T>()
	{
	}

	template <class T> ArrayListNative<T>::ArrayListNative(UIntOS capacity) : ArrayListNativeBase<T>(capacity)
	{
	}

	template <class T> ArrayListNative<T>::ArrayListNative(const ArrayListNative<T> &list) : ArrayListNativeBase<T>(list.capacity)
	{
		this->AddAll(list);
	}

	template <class T> ArrayListNative<T>::~ArrayListNative()
	{
	}

	template <class T> UIntOS ArrayListNative<T>::Add(T val)
	{
		UIntOS ret;
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

	template <class T> UIntOS ArrayListNative<T>::AddAll(NN<const ReadingList<T>> arr)
	{
		UIntOS cnt = arr->GetCount();
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
		UIntOS i = 0;
		while (i < cnt)
		{
			this->arr[this->objCnt + i] = arr->GetItem(i);
			i++;
		}
		this->objCnt += cnt;
		return cnt;
	}

	template <class T> UIntOS ArrayListNative<T>::AddRange(UnsafeArray<const T> arr, UIntOS cnt)
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

	template <class T> T ArrayListNative<T>::RemoveAt(UIntOS index)
	{
		if (index >= this->objCnt)
			return (T)0;
		UIntOS i = this->objCnt - index - 1;
		T o = this->arr[index];
		if (i > 0)
		{
			MemCopyO(&this->arr[index], &this->arr[index + 1], i * sizeof(T));
		}
		this->objCnt--;
		//arr[objCnt] = (T)0;
		return o;
	}

	template <class T> void ArrayListNative<T>::Insert(UIntOS index, T Val)
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

	template <class T> NN<ArrayListNative<T>> ArrayListNative<T>::Clone() const
	{
		NN<ArrayListNative<T>> newArr;
		NEW_CLASSNN(newArr, ArrayListNative<T>(this->capacity));
		newArr->AddAll(NNTHIS);
		return newArr;
	}

	template <class T> void ArrayListNative<T>::EnsureCapacity(UIntOS capacity)
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

	template <class T> UIntOS ArrayListNative<T>::GetRange(UnsafeArray<T> outArr, UIntOS index, UIntOS cnt) const
	{
		UIntOS startIndex = index;
		UIntOS endIndex = index + cnt;
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

	template <class T> UIntOS ArrayListNative<T>::RemoveRange(UIntOS index, UIntOS cnt)
	{
		UIntOS startIndex = index;
		UIntOS endIndex = index + cnt;
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
		UIntOS i = startIndex;
		UIntOS j = endIndex;
		while (j < this->objCnt)
		{
			this->arr[i++] = this->arr[j++];
		}
#endif
		this->objCnt -= endIndex - startIndex;
		return endIndex - startIndex;
	}

	template <class T> ArrayListNative<T> &ArrayListNative<T>::operator =(const ArrayListNative<T> &v)
	{
		this->Clear();
		this->EnsureCapacity(v.capacity);
		this->AddAll(v);
		return NNTHIS;
	}

}
#endif
