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
		UOSInt objCnt;
		UOSInt capacity;

		void Init(UOSInt capacity);
	public:
		ArrayListArr();
		ArrayListArr(UOSInt capacity);
		ArrayListArr(const ArrayListArr<T> &list);
		virtual ~ArrayListArr();

		virtual UOSInt Add(UnsafeArray<T> val);
		UOSInt AddAll(NN<const ReadingListArr<T>> arr);
		virtual UOSInt AddRange(UnsafeArray<UnsafeArray<T>> arr, UOSInt cnt);
		virtual Bool Remove(UnsafeArray<T> val);
		virtual UnsafeArrayOpt<T> RemoveAt(UOSInt index);
		virtual void Insert(UOSInt index, UnsafeArray<T> val);
		virtual UOSInt IndexOf(UnsafeArray<T> val) const;
		virtual void Clear();
		virtual NN<ArrayListArr<T>> Clone() const;

		virtual UOSInt GetCount() const;
		virtual UOSInt GetCapacity() const;
		void EnsureCapacity(UOSInt capacity);

		virtual UnsafeArrayOpt<T> GetItem(UOSInt index) const;
		virtual UnsafeArray<T> GetItemNoCheck(UOSInt index) const;
		virtual void SetItem(UOSInt index, UnsafeArray<T> val);
		void CopyItems(UOSInt destIndex, UOSInt srcIndex, UOSInt count);
		UOSInt GetRange(UnsafeArray<T> outArr, UOSInt index, UOSInt cnt) const;
		UOSInt RemoveRange(UOSInt index, UOSInt cnt);
		virtual UnsafeArray<UnsafeArray<T>> GetArr(OutParam<UOSInt> arraySize) const;
		virtual UnsafeArray<UnsafeArray<T>> Arr() const;
		UnsafeArrayOpt<T> Pop();
		ArrayListArr<T> &operator =(const ArrayListArr<T> &v);
		void Reverse();
	};


	template <class T> void ArrayListArr<T>::Init(UOSInt capacity)
	{
		objCnt = 0;
		this->capacity = capacity;
		arr = MemAllocArr(UnsafeArray<T>, capacity);
	}

	template <class T> ArrayListArr<T>::ArrayListArr()
	{
		Init(40);
	}

	template <class T> ArrayListArr<T>::ArrayListArr(UOSInt capacity)
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

	template <class T> UOSInt ArrayListArr<T>::Add(UnsafeArray<T> val)
	{
		UOSInt ret;
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

	template <class T> UOSInt ArrayListArr<T>::AddAll(NN<const ReadingListArr<T>> arr)
	{
		UOSInt cnt = arr->GetCount();
		if (objCnt + cnt >= this->capacity)
		{
			while (objCnt + cnt >= this->capacity)
			{
				this->capacity = this->capacity << 1;
			}
			UnsafeArray<UnsafeArray<T>> newArr = MemAllocArr(UnsafeArray<T>, this->capacity);
			if (objCnt > 0)
			{
				UOSInt i = 0;
				while (i < objCnt)
				{
					newArr[i] = this->arr[i];
					i++;
				}
			}
			MemFreeArr(this->arr);
			this->arr = newArr;
		}
		UOSInt i = 0;
		while (i < cnt)
		{
			this->arr[this->objCnt + i] = arr->GetItemNoCheck(i);
			i++;
		}
		this->objCnt += cnt;
		return cnt;
	}

	template <class T> UOSInt ArrayListArr<T>::AddRange(UnsafeArray<UnsafeArray<T>> arr, UOSInt cnt)
	{
		UOSInt i;
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

	template <class T> UnsafeArrayOpt<T> ArrayListArr<T>::RemoveAt(UOSInt index)
	{
		if (index >= this->objCnt)
			return 0;
		UOSInt i = this->objCnt - index - 1;
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

	template <class T> void ArrayListArr<T>::Insert(UOSInt index, UnsafeArray<T> val)
	{
		UOSInt i;
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
			UOSInt j = this->objCnt;
			while (j > index)
			{
				this->arr[j] = this->arr[j - 1];
				j--;
			}
			arr[index] = val;
		}
		objCnt++;
	}

	template <class T> UOSInt ArrayListArr<T>::IndexOf(UnsafeArray<T> val) const
	{
		UOSInt i = objCnt;
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
		newArr->AddAll(*this);
		return newArr;
	}

	template <class T> UOSInt ArrayListArr<T>::GetCount() const
	{
		return this->objCnt;
	}

	template <class T> UOSInt ArrayListArr<T>::GetCapacity() const
	{
		return this->capacity;
	}

	template <class T> void ArrayListArr<T>::EnsureCapacity(UOSInt capacity)
	{
		if (capacity > this->capacity)
		{
			while (capacity > this->capacity)
			{
				this->capacity = this->capacity << 1;
			}

			UnsafeArray<T> newArr = MemAllocArr(T, this->capacity);
			UOSInt i = 0;
			while (i < this->objCnt)
			{
				newArr[i] = this->arr[i];
				i++;
			}
			MemFreeArr(this->arr);
			this->arr = newArr;
		}
	}

	template <class T> UnsafeArrayOpt<T> ArrayListArr<T>::GetItem(UOSInt index) const
	{
		if (index >= this->objCnt || index < 0)
			return 0;
		return this->arr[index];
	}

	template <class T> UnsafeArray<T> ArrayListArr<T>::GetItemNoCheck(UOSInt index) const
	{
		return this->arr[index];
	}

	template <class T> void ArrayListArr<T>::SetItem(UOSInt index, UnsafeArray<T> val)
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

	template <class T> void ArrayListArr<T>::CopyItems(UOSInt destIndex, UOSInt srcIndex, UOSInt count)
	{
		UOSInt i;
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

	template <class T> UOSInt ArrayListArr<T>::GetRange(UnsafeArray<T> outArr, UOSInt Index, UOSInt cnt) const
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

	template <class T> UOSInt ArrayListArr<T>::RemoveRange(UOSInt index, UOSInt cnt)
	{
		UOSInt startIndex = index;
		UOSInt endIndex = index + cnt;
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
		UOSInt i = startIndex;
		UOSInt j = endIndex;
		while (j < objCnt)
		{
			arr[i++] = arr[j++];
		}
		objCnt -= endIndex - startIndex;
		return endIndex - startIndex;
	}

	template <class T> UnsafeArray<UnsafeArray<T>> ArrayListArr<T>::GetArr(OutParam<UOSInt> arraySize) const
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
		return *this;
	}

	template <class T> void ArrayListArr<T>::Reverse()
	{
		NN<T> tmp;
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
