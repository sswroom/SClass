#ifndef _SM_DATA_ARRAYLISTARR
#define _SM_DATA_ARRAYLISTARR
#include "MemTool.h"
#include "MyMemory.h"
#include "Data/ArrayCollection.hpp"
#include "Data/ReadingListArr.hpp"

namespace Data
{
	template <class T> class ArrayListArr : public ReadingListArr<T>, public ArrayCollection<UnsafeArray<T>>
	{
	protected:
		UnsafeArray<UnsafeArray<T>> arr;
		UIntOS objCnt;
		UIntOS capacity;

		void Init(UIntOS capacity);
	public:
		ArrayListArr();
		ArrayListArr(UIntOS capacity);
		ArrayListArr(const ArrayListArr<T> &list);
		virtual ~ArrayListArr();

		virtual UIntOS Add(UnsafeArray<T> val);
		UIntOS AddAll(NN<const ReadingListArr<T>> arr);
		virtual UIntOS AddRange(UnsafeArray<UnsafeArray<T>> arr, UIntOS cnt);
		virtual Bool Remove(UnsafeArray<T> val);
		virtual UnsafeArrayOpt<T> RemoveAt(UIntOS index);
		virtual void Insert(UIntOS index, UnsafeArray<T> val);
		virtual UIntOS IndexOf(UnsafeArray<T> val) const;
		virtual void Clear();
		virtual NN<ArrayListArr<T>> Clone() const;

		virtual UIntOS GetCount() const;
		virtual UIntOS GetCapacity() const;
		void EnsureCapacity(UIntOS capacity);

		virtual UnsafeArrayOpt<T> GetItem(UIntOS index) const;
		virtual UnsafeArray<T> GetItemNoCheck(UIntOS index) const;
		virtual void SetItem(UIntOS index, UnsafeArray<T> val);
		void CopyItems(UIntOS destIndex, UIntOS srcIndex, UIntOS count);
		UIntOS GetRange(UnsafeArray<T> outArr, UIntOS index, UIntOS cnt) const;
		UIntOS RemoveRange(UIntOS index, UIntOS cnt);
		virtual UnsafeArray<UnsafeArray<T>> GetArr(OutParam<UIntOS> arraySize) const;
		virtual UnsafeArray<UnsafeArray<T>> Arr() const;
		UnsafeArrayOpt<T> Pop();
		ArrayListArr<T> &operator =(const ArrayListArr<T> &v);
		void Reverse();
	};


	template <class T> void ArrayListArr<T>::Init(UIntOS capacity)
	{
		objCnt = 0;
		this->capacity = capacity;
		arr = MemAllocArr(UnsafeArray<T>, capacity);
	}

	template <class T> ArrayListArr<T>::ArrayListArr()
	{
		Init(40);
	}

	template <class T> ArrayListArr<T>::ArrayListArr(UIntOS capacity)
	{
		Init(capacity);
	}

	template <class T> ArrayListArr<T>::ArrayListArr(const ArrayListArr<T> &list)
	{
		Init(list.capacity);
		this->AddAll(list);
	}

	template <class T> ArrayListArr<T>::~ArrayListArr()
	{
		MemFreeArr(arr);
	}

	template <class T> UIntOS ArrayListArr<T>::Add(UnsafeArray<T> val)
	{
		UIntOS ret;
		if (objCnt == this->capacity)
		{
			UnsafeArray<UnsafeArray<T>> newArr = MemAllocArr(UnsafeArray<T>, this->capacity << 1);
			ret = 0;
			while (ret < this->objCnt)
			{
				newArr[ret] = this->arr[ret];
				ret++;
			}
			this->capacity = this->capacity << 1;
			MemFreeArr(arr);
			arr = newArr;
		}
		arr[ret = objCnt++] = val;
		return ret;
	}

	template <class T> UIntOS ArrayListArr<T>::AddAll(NN<const ReadingListArr<T>> arr)
	{
		UIntOS cnt = arr->GetCount();
		if (objCnt + cnt >= this->capacity)
		{
			while (objCnt + cnt >= this->capacity)
			{
				this->capacity = this->capacity << 1;
			}
			UnsafeArray<UnsafeArray<T>> newArr = MemAllocArr(UnsafeArray<T>, this->capacity);
			if (objCnt > 0)
			{
				UIntOS i = 0;
				while (i < objCnt)
				{
					newArr[i] = this->arr[i];
					i++;
				}
			}
			MemFreeArr(this->arr);
			this->arr = newArr;
		}
		UIntOS i = 0;
		while (i < cnt)
		{
			this->arr[this->objCnt + i] = arr->GetItemNoCheck(i);
			i++;
		}
		this->objCnt += cnt;
		return cnt;
	}

	template <class T> UIntOS ArrayListArr<T>::AddRange(UnsafeArray<UnsafeArray<T>> arr, UIntOS cnt)
	{
		UIntOS i;
		if (objCnt + cnt >= this->capacity)
		{
			while (objCnt + cnt >= this->capacity)
			{
				this->capacity = this->capacity << 1;
			}
			UnsafeArray<UnsafeArray<T>> newArr = MemAllocArr(UnsafeArray<T>, this->capacity);
			if (objCnt > 0)
			{
				i = 0;
				while (i < objCnt)
				{
					newArr[i] = this->arr[i];
					i++;
				}
			}
			MemFreeArr(this->arr);
			this->arr = newArr;
		}
		i = 0;
		while (i < cnt)
		{
			this->arr[objCnt + i] = arr[i];
			i++;
		}
		this->objCnt += cnt;
		return cnt;
	}

	template <class T> Bool ArrayListArr<T>::Remove(UnsafeArray<T> val)
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

	template <class T> UnsafeArrayOpt<T> ArrayListArr<T>::RemoveAt(UIntOS index)
	{
		if (index >= this->objCnt)
			return nullptr;
		UIntOS i = this->objCnt - index - 1;
		UnsafeArray<T> o = arr[index];
		if (i > 0)
		{
			while (i-- > 0)
			{
				arr[index] = arr[index + 1];
				index++;
			}
		}
		this->objCnt--;
		//arr[objCnt] = (T)0;
		return o;
	}

	template <class T> void ArrayListArr<T>::Insert(UIntOS index, UnsafeArray<T> val)
	{
		UIntOS i;
		if (objCnt == this->capacity)
		{
			UnsafeArray<UnsafeArray<T>> newArr = MemAllocArr(UnsafeArray<T>, this->capacity << 1);
			if (index > 0)
			{
				i = 0;
				while (i < index)
				{
					newArr[i] = this->arr[i];
					i++;
				}
			}
			newArr[index] = val;
			if (index < this->objCnt)
			{
				i = this->objCnt;
				while (i-- > index)
				{
					newArr[index + 1] = this->arr[index];
				}
			}
			this->capacity = this->capacity << 1;
			MemFreeArr(arr);
			arr = newArr;
		}
		else
		{
			UIntOS j = this->objCnt;
			while (j > index)
			{
				this->arr[j] = this->arr[j - 1];
				j--;
			}
			arr[index] = val;
		}
		objCnt++;
	}

	template <class T> UIntOS ArrayListArr<T>::IndexOf(UnsafeArray<T> val) const
	{
		UIntOS i = objCnt;
		while (i-- > 0)
			if (arr[i] == val)
				return i;
		return INVALID_INDEX;
	}

	template <class T> void ArrayListArr<T>::Clear()
	{
		this->objCnt = 0;
	}

	template <class T> NN<ArrayListArr<T>> ArrayListArr<T>::Clone() const
	{
		NN<ArrayListArr<T>> newArr;
		NEW_CLASSNN(newArr, ArrayListArr<T>(this->capacity));
		newArr->AddAll(NNTHIS);
		return newArr;
	}

	template <class T> UIntOS ArrayListArr<T>::GetCount() const
	{
		return this->objCnt;
	}

	template <class T> UIntOS ArrayListArr<T>::GetCapacity() const
	{
		return this->capacity;
	}

	template <class T> void ArrayListArr<T>::EnsureCapacity(UIntOS capacity)
	{
		if (capacity > this->capacity)
		{
			while (capacity > this->capacity)
			{
				this->capacity = this->capacity << 1;
			}

			UnsafeArray<T> newArr = MemAllocArr(T, this->capacity);
			UIntOS i = 0;
			while (i < this->objCnt)
			{
				newArr[i] = this->arr[i];
				i++;
			}
			MemFreeArr(this->arr);
			this->arr = newArr;
		}
	}

	template <class T> UnsafeArrayOpt<T> ArrayListArr<T>::GetItem(UIntOS index) const
	{
		if (index >= this->objCnt || index < 0)
			return nullptr;
		return this->arr[index];
	}

	template <class T> UnsafeArray<T> ArrayListArr<T>::GetItemNoCheck(UIntOS index) const
	{
		return this->arr[index];
	}

	template <class T> void ArrayListArr<T>::SetItem(UIntOS index, UnsafeArray<T> val)
	{
		if (index == objCnt)
		{
			Add(val);
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

	template <class T> void ArrayListArr<T>::CopyItems(UIntOS destIndex, UIntOS srcIndex, UIntOS count)
	{
		UIntOS i;
		if (destIndex > srcIndex)
		{
			i = count;
			while (i-- > 0)
			{
				this->arr[destIndex + i] = this->arr[srcIndex + i];
			}
		}
		else if (destIndex < srcIndex)
		{
			i = 0;
			while (i < count)
			{
				this->arr[destIndex + i] = this->arr[srcIndex + i];
				i++;
			}
		}
	}

	template <class T> UIntOS ArrayListArr<T>::GetRange(UnsafeArray<T> outArr, UIntOS Index, UIntOS cnt) const
	{
		UIntOS startIndex = Index;
		UIntOS endIndex = Index + cnt;
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

	template <class T> UIntOS ArrayListArr<T>::RemoveRange(UIntOS index, UIntOS cnt)
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
		UIntOS i = startIndex;
		UIntOS j = endIndex;
		while (j < objCnt)
		{
			arr[i++] = arr[j++];
		}
		objCnt -= endIndex - startIndex;
		return endIndex - startIndex;
	}

	template <class T> UnsafeArray<UnsafeArray<T>> ArrayListArr<T>::GetArr(OutParam<UIntOS> arraySize) const
	{
		arraySize.Set(this->objCnt);
		return this->arr;
	}

	template <class T> UnsafeArray<UnsafeArray<T>> ArrayListArr<T>::Arr() const
	{
		return this->arr;
	}

	template <class T> UnsafeArrayOpt<T> ArrayListArr<T>::Pop()
	{
		if (this->objCnt == 0) return 0;
		UnsafeArray<T> o = arr[this->objCnt - 1];
		this->objCnt--;
		return o;
	}

	template <class T> ArrayListArr<T> &ArrayListArr<T>::operator =(const ArrayListArr<T> &v)
	{
		this->Clear();
		this->EnsureCapacity(v.capacity);
		this->AddAll(v);
		return NNTHIS;
	}

	template <class T> void ArrayListArr<T>::Reverse()
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
