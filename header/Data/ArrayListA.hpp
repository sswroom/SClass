#ifndef _SM_DATA_ARRAYLISTA
#define _SM_DATA_ARRAYLISTA
#include "MemTool.h"
#include "MyMemory.h"
#include "Data/ArrayCollection.hpp"
#include "Data/List.hpp"

namespace Data
{
	// ArrayList of aligned data type
	template <class T> class ArrayListA : public List<T>, public ArrayCollection<T>
	{
	protected:
		UnsafeArray<T> arr;
		UIntOS objCnt;
		UIntOS capacity;

		void Init(UIntOS capacity);
	public:
		ArrayListA();
		ArrayListA(UIntOS capacity);
		virtual ~ArrayListA();

		virtual UIntOS Add(T val);
		UIntOS AddAll(NN<const ReadingList<T>> arr);
		virtual UIntOS AddRange(UnsafeArray<const T> arr, UIntOS cnt);
		virtual Bool Remove(T val);
		virtual T RemoveAt(UIntOS index);
		virtual void Insert(UIntOS index, T val);
		virtual UIntOS IndexOf(T val) const;
		virtual void Clear();
		virtual NN<ArrayListA<T>> Clone() const;

		virtual UIntOS GetCount() const;
		virtual UIntOS GetCapacity() const;
		void EnsureCapacity(UIntOS capacity);

		virtual T GetItem(UIntOS index) const;
		virtual void SetItem(UIntOS index, T val);
		void CopyItems(UIntOS destIndex, UIntOS srcIndex, UIntOS count);
		UIntOS GetRange(UnsafeArray<T> outArr, UIntOS index, UIntOS cnt) const;
		UIntOS RemoveRange(UIntOS index, UIntOS cnt);
		void InsertRange(UIntOS index, UIntOS cnt, UnsafeArray<const T> arr);
		virtual UnsafeArray<T> GetArr(OutParam<UIntOS> arraySize) const;
		virtual UnsafeArray<T> Arr() const;
		T Pop();
		void Reverse();
	};


	template <class T> void ArrayListA<T>::Init(UIntOS Capacity)
	{
		objCnt = 0;
		this->capacity = Capacity;
		arr = MemAllocA(T, Capacity);
	}

	template <class T> ArrayListA<T>::ArrayListA()
	{
		Init(40);
	}

	template <class T> ArrayListA<T>::ArrayListA(UIntOS Capacity)
	{
		Init(Capacity);
	}

	template <class T> ArrayListA<T>::~ArrayListA()
	{
		MemFreeAArr(arr);
	}

	template <class T> UIntOS ArrayListA<T>::Add(T val)
	{
		UIntOS ret;
		if (objCnt == this->capacity)
		{
			UnsafeArray<T> newArr = MemAllocAArr(T, this->capacity << 1);
			MemCopyAC(newArr.Ptr(), arr.Ptr(), this->objCnt * sizeof(T));
			this->capacity = this->capacity << 1;
			MemFreeAArr(arr);
			arr = newArr;
		}
		arr[ret = objCnt++] = val;
		return ret;
	}

	template <class T> UIntOS ArrayListA<T>::AddAll(NN<const ReadingList<T>> arr)
	{
		UIntOS cnt = arr->GetCount();
		if (objCnt + cnt >= this->capacity)
		{
			while (objCnt + cnt >= this->capacity)
			{
				this->capacity = this->capacity << 1;
			}
			UnsafeArray<T> newArr = MemAllocAArr(T, this->capacity);
			if (objCnt > 0)
			{
				MemCopyAC(newArr.Ptr(), this->arr.Ptr(), objCnt * sizeof(T));
			}
			MemFreeAArr(this->arr);
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

	template <class T> UIntOS ArrayListA<T>::AddRange(UnsafeArray<const T> arr, UIntOS cnt)
	{
		if (objCnt + cnt >= this->capacity)
		{
			while (objCnt + cnt >= this->capacity)
			{
				this->capacity = this->capacity << 1;
			}
			UnsafeArray<T> newArr = MemAllocAArr(T, this->capacity);
			if (objCnt > 0)
			{
				MemCopyAC(newArr.Ptr(), this->arr.Ptr(), objCnt * sizeof(T));
			}
			MemFreeAArr(this->arr);
			this->arr = newArr;
		}
		MemCopyNO(&this->arr[objCnt], arr.Ptr(), cnt * sizeof(T));
		this->objCnt += cnt;
		return cnt;
	}

	template <class T> Bool ArrayListA<T>::Remove(T val)
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

	template <class T> T ArrayListA<T>::RemoveAt(UIntOS index)
	{
		if (index >= this->objCnt)
			return (T)0;
		UIntOS i = this->objCnt - index - 1;
		T o = arr[index];
		if (i > 0)
		{
			MemCopyO(&arr[index], &arr[index + 1], i * sizeof(T));
		}
		this->objCnt--;
		//arr[objCnt] = (T)0;
		return o;
	}

	template <class T> void ArrayListA<T>::Insert(UIntOS Index, T Val)
	{
		if (objCnt == this->capacity)
		{
			UnsafeArray<T> newArr = MemAllocAArr(T, this->capacity << 1);
			if (Index > 0)
			{
				MemCopyAC(newArr.Ptr(), this->arr.Ptr(), Index * sizeof(T));
			}
			newArr[Index] = Val;
			if (Index < this->objCnt)
			{
				MemCopyNO(&newArr[Index + 1], &this->arr[Index], (this->objCnt - Index) * sizeof(T));
			}
			this->capacity = this->capacity << 1;
			MemFreeAArr(arr);
			arr = newArr;
		}
		else
		{
			UIntOS j = this->objCnt;
			while (j > Index)
			{
				this->arr[j] = this->arr[j - 1];
				j--;
			}
			arr[Index] = Val;
		}
		objCnt++;
	}

	template <class T> UIntOS ArrayListA<T>::IndexOf(T val) const
	{
		UIntOS i = objCnt;
		while (i-- > 0)
			if (arr[i] == val)
				return i;
		return INVALID_INDEX;
	}

	template <class T> void ArrayListA<T>::Clear()
	{
		UIntOS i = objCnt;
		while (i-- > 0)
		{
			arr[i] = (T)0;
		}
		this->objCnt = 0;
	}

	template <class T> NN<ArrayListA<T>> ArrayListA<T>::Clone() const
	{
		NN<ArrayListA<T>> newArr;
		NEW_CLASSNN(newArr, ArrayListA<T>(this->capacity));
		newArr->AddAll(NNTHIS);
		return newArr;
	}

	template <class T> UIntOS ArrayListA<T>::GetCount() const
	{
		return this->objCnt;
	}

	template <class T> UIntOS ArrayListA<T>::GetCapacity() const
	{
		return this->capacity;
	}

	template <class T> void ArrayListA<T>::EnsureCapacity(UIntOS Capacity)
	{
		if (Capacity > this->capacity)
		{
			while (Capacity > this->capacity)
			{
				this->capacity = this->capacity << 1;
			}

			UnsafeArray<T> newArr = MemAllocAArr(T, this->capacity);
			MemCopyAC(newArr.Ptr(), this->arr.Ptr(), this->objCnt * sizeof(T));
			MemFreeAArr(this->arr);
			this->arr = newArr;
		}
	}

	template <class T> T ArrayListA<T>::GetItem(UIntOS Index) const
	{
		if (Index >= this->objCnt || Index < 0)
			return (T)0;
		return this->arr[Index];
	}

	template <class T> void ArrayListA<T>::SetItem(UIntOS Index, T Val)
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

	template <class T> void ArrayListA<T>::CopyItems(UIntOS destIndex, UIntOS srcIndex, UIntOS count)
	{
		MemCopyO(&this->arr[destIndex], &this->arr[srcIndex], count * sizeof(this->arr[0]));
	}

	template <class T> UIntOS ArrayListA<T>::GetRange(UnsafeArray<T> outArr, UIntOS index, UIntOS cnt) const
	{
		UIntOS startIndex = index;
		UIntOS endIndex = index + cnt;
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

	template <class T> UIntOS ArrayListA<T>::RemoveRange(UIntOS index, UIntOS cnt)
	{
		UIntOS startIndex = index;
		UIntOS endIndex = index + cnt;
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
		UIntOS i = startIndex;
		UIntOS j = endIndex;
		while (j < objCnt)
		{
			arr[i++] = arr[j++];
		}
#endif
		objCnt -= endIndex - startIndex;
		return endIndex - startIndex;
	}

	template <class T> void ArrayListA<T>::InsertRange(UIntOS index, UIntOS cnt, UnsafeArray<const T> arr)
	{
		if (index > this->objCnt)
		{
			index = this->objCnt;
		}
		if (objCnt + cnt >= this->capacity)
		{
			while (objCnt + cnt >= this->capacity)
			{
				this->capacity = this->capacity << 1;
			}
			UnsafeArray<T> newArr = MemAllocAArr(T, this->capacity);
			if (index > 0)
			{
				MemCopyNO(newArr.Ptr(), this->arr.Ptr(), index * sizeof(T));
			}
			MemCopyNO(&newArr[index], arr.Ptr(), cnt * sizeof(T));
			if (objCnt > index)
			{
				MemCopyNO(&newArr[index + cnt], &this->arr[index], (objCnt - index) * sizeof(T));
			}
			MemFreeAArr(this->arr);
			this->arr = newArr;
		}
		else
		{
			if (this->objCnt > index)
			{
				MemCopyO(&this->arr[index + cnt], &this->arr[index], (objCnt - index) * sizeof(T));
			}
			MemCopyNO(&this->arr[index], arr.Ptr(), cnt * sizeof(T));
		}
		this->objCnt += cnt;
	}

	template <class T> UnsafeArray<T> ArrayListA<T>::GetArr(OutParam<UIntOS> arraySize) const
	{
		arraySize.Set(this->objCnt);
		return this->arr;
	}
	
	template <class T> UnsafeArray<T> ArrayListA<T>::Arr() const
	{
		return this->arr;
	}
	
	template <class T> T ArrayListA<T>::Pop()
	{
		if (this->objCnt == 0) return 0;
		T o = arr[this->objCnt - 1];
		this->objCnt--;
		return o;
	}

	template <class T> void ArrayListA<T>::Reverse()
	{
		NN<T> tmp;
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
