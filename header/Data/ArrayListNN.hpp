#ifndef _SM_DATA_ARRAYLISTNN
#define _SM_DATA_ARRAYLISTNN
#include "MemTool.h"
#include "MyMemory.h"
#include "Data/ArrayCollection.hpp"
#include "Data/DataArray.hpp"
#include "Data/ReadingList.hpp"
#include "Data/ReadingListNN.hpp"

namespace Data
{
	template <class T> class ArrayListNN : public ReadingListNN<T>, public ArrayCollection<NN<T>>
	{
	public:
		typedef void (CALLBACKFUNC FreeFunc)(NN<T> v);
	protected:
		UnsafeArray<NN<T>> arr;
		UIntOS objCnt;
		UIntOS capacity;

		void Init(UIntOS capacity);
	public:
		ArrayListNN();
		ArrayListNN(UIntOS capacity);
		ArrayListNN(const ArrayListNN<T> &list);
		virtual ~ArrayListNN();

		virtual UIntOS Add(NN<T> val);
		virtual UIntOS AddRange(UnsafeArray<const NN<T>> arr, UIntOS cnt);
		UIntOS AddAll(NN<const ReadingListNN<T>> list);
		UIntOS AddAll(Data::ArrayIterator<NN<T>> it);
		UIntOS AddAllOpt(NN<const ReadingList<Optional<T>>> list);
		virtual Bool Remove(NN<T> val);
		virtual Optional<T> RemoveAt(UIntOS index);
		virtual void Insert(UIntOS index, NN<T> val);
		virtual UIntOS IndexOf(NN<T> val) const;
		virtual void Clear();
		virtual NN<ArrayListNN<T>> Clone() const;

		virtual UIntOS GetCount() const;
		virtual UIntOS GetCapacity() const;
		void EnsureCapacity(UIntOS capacity);

		virtual Optional<T> GetItem(UIntOS index) const;
		virtual NN<T> GetItemNoCheck(UIntOS index) const;
		virtual Optional<T> SetItem(UIntOS index, NN<T> val);
		void CopyItems(UIntOS destIndex, UIntOS srcIndex, UIntOS count);
		UIntOS GetRange(UnsafeArray<NN<T>> outArr, UIntOS index, UIntOS cnt) const;
		UIntOS RemoveRange(UIntOS index, UIntOS cnt);
		virtual UnsafeArray<NN<T>> GetArr(OutParam<UIntOS> arraySize) const;
		virtual UnsafeArray<NN<T>> Arr() const;
		Optional<T> GetLast();
		Optional<T> Pop();
		ArrayListNN<T> &operator =(const ArrayListNN<T> &v);
		void DeleteAll();
		void FreeAll(FreeFunc freeFunc);
		void MemFreeAll();
		Data::DataArray<NN<T>> ToArray() const;
		void Reverse();
	};


	template <class T> void ArrayListNN<T>::Init(UIntOS capacity)
	{
		this->objCnt = 0;
		this->capacity = capacity;
		this->arr = MemAllocArr(NN<T>, capacity);
	}

	template <class T> ArrayListNN<T>::ArrayListNN()
	{
		Init(40);
	}

	template <class T> ArrayListNN<T>::ArrayListNN(UIntOS capacity)
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
		MemFreeArr(arr);
	}

	template <class T> UIntOS ArrayListNN<T>::Add(NN<T> val)
	{
		UIntOS ret;
		if (objCnt >= this->capacity)
		{
			this->capacity = this->capacity << 1;
			UnsafeArray<NN<T>> newArr = MemAllocArr(NN<T>, this->capacity);
			newArr.CopyFromNO(arr, this->objCnt);
			MemFreeArr(arr);
			arr = newArr;
		}
		arr[ret = objCnt++] = val;
		return ret;
	}

	template <class T> UIntOS ArrayListNN<T>::AddAll(NN<const ReadingListNN<T>> arr)
	{
		UIntOS cnt = arr->GetCount();
		if (objCnt + cnt >= this->capacity)
		{
			while (objCnt + cnt >= this->capacity)
			{
				this->capacity = this->capacity << 1;
			}
			UnsafeArray<NN<T>> newArr = MemAllocArr(NN<T>, this->capacity);
			if (objCnt > 0)
			{
				newArr.CopyFromNO(this->arr, objCnt);
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

	template <class T> UIntOS ArrayListNN<T>::AddAll(Data::ArrayIterator<NN<T>> it)
	{
		UIntOS ret = 0;
		while (it->HasNext())
		{
			this->Add(it->Next());
			ret++;
		}
		return ret;
	}

	template <class T> UIntOS ArrayListNN<T>::AddAllOpt(NN<const ReadingList<Optional<T>>> list)
	{
		UIntOS ret = 0;
		UIntOS i = 0;
		UIntOS j = list->GetCount();
		NN<T> s;
		while (i < j)
		{
			if (list->GetItem(i).SetTo(s))
			{
				this->Add(s);
				ret++;
			}
			i++;
		}
		return ret;
	}

	template <class T> UIntOS ArrayListNN<T>::AddRange(UnsafeArray<const NN<T>> arr, UIntOS cnt)
	{
		if (objCnt + cnt >= this->capacity)
		{
			while (objCnt + cnt >= this->capacity)
			{
				this->capacity = this->capacity << 1;
			}
			UnsafeArray<NN<T>> newArr = MemAllocArr(NN<T>, this->capacity);
			if (objCnt > 0)
			{
				newArr.CopyFromNO(this->arr, objCnt);
			}
			MemFreeArr(this->arr);
			this->arr = newArr;
		}
		MemCopyNO(&this->arr[objCnt], arr.Ptr(), cnt * sizeof(NN<T>));
		this->objCnt += cnt;
		return cnt;
	}

	template <class T> Bool ArrayListNN<T>::Remove(NN<T> val)
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

	template <class T> Optional<T> ArrayListNN<T>::RemoveAt(UIntOS index)
	{
		if (index >= this->objCnt)
			return nullptr;
		UIntOS i = this->objCnt - index - 1;
		NN<T> o = arr[index];
		if (i > 0)
		{
			MemCopyO(&arr[index], &arr[index + 1], i * sizeof(NN<T>));
		}
		this->objCnt--;
		//arr[objCnt] = (T)0;
		return o;
	}

	template <class T> void ArrayListNN<T>::Insert(UIntOS index, NN<T> Val)
	{
		if (objCnt == this->capacity)
		{
			UnsafeArray<NN<T>> newArr = MemAllocArr(NN<T>, this->capacity << 1);
			if (index > 0)
			{
				newArr.CopyFromNO(this->arr, index);
			}
			newArr[index] = Val;
			if (index < this->objCnt)
			{
				MemCopyNO(&newArr[index + 1], &this->arr[index], (this->objCnt - index) * sizeof(NN<T>));
			}
			this->capacity = this->capacity << 1;
			MemFreeArr(arr);
			arr = newArr;
		}
		else
		{
			UIntOS j = this->objCnt;
			if (j > index)
			{
				MemCopyO(&this->arr[index + 1], &this->arr[index], (j - index) * sizeof(NN<T>));
			}
/*			while (j > index)
			{
				this->arr[j] = this->arr[j - 1];
				j--;
			}*/
			this->arr[index] = Val;
		}
		objCnt++;
	}

	template <class T> UIntOS ArrayListNN<T>::IndexOf(NN<T> val) const
	{
		UIntOS i = objCnt;
		while (i-- > 0)
			if (arr[i] == val)
				return i;
		return INVALID_INDEX;
	}

	template <class T> void ArrayListNN<T>::Clear()
	{
		this->objCnt = 0;
	}

	template <class T> NN<ArrayListNN<T>> ArrayListNN<T>::Clone() const
	{
		NN<ArrayListNN<T>> newArr;
		NEW_CLASSNN(newArr, ArrayListNN<T>(this->capacity));
		newArr->AddAll(NNTHIS);
		return newArr;
	}

	template <class T> UIntOS ArrayListNN<T>::GetCount() const
	{
		return this->objCnt;
	}

	template <class T> UIntOS ArrayListNN<T>::GetCapacity() const
	{
		return this->capacity;
	}

	template <class T> void ArrayListNN<T>::EnsureCapacity(UIntOS capacity)
	{
		if (capacity > this->capacity)
		{
			while (capacity > this->capacity)
			{
				this->capacity = this->capacity << 1;
			}

			UnsafeArray<NN<T>> newArr = MemAllocArr(NN<T>, this->capacity);
			newArr.CopyFromNO(this->arr, this->objCnt);
			MemFreeArr(this->arr);
			this->arr = newArr;
		}
	}

	template <class T> Optional<T> ArrayListNN<T>::GetItem(UIntOS index) const
	{
		if (index >= this->objCnt || index < 0)
			return nullptr;
		return this->arr[index];
	}

	template <class T> NN<T> ArrayListNN<T>::GetItemNoCheck(UIntOS index) const
	{
		return this->arr[index];
	}

	template <class T> Optional<T> ArrayListNN<T>::SetItem(UIntOS index, NN<T> val)
	{
		if (index == objCnt)
		{
			Add(val);
			return Optional<T>(nullptr);
		}
		else if (index < objCnt)
		{
			NN<T> oriVal = this->arr[index];
			this->arr[index] = val;
			return oriVal;
		}
		else
		{
			return Optional<T>(nullptr);
		}
	}

	template <class T> void ArrayListNN<T>::CopyItems(UIntOS destIndex, UIntOS srcIndex, UIntOS count)
	{
		MemCopyO(&this->arr[destIndex], &this->arr[srcIndex], count * sizeof(this->arr[0]));
	}

	template <class T> UIntOS ArrayListNN<T>::GetRange(UnsafeArray<NN<T>> outArr, UIntOS Index, UIntOS cnt) const
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
		MemCopyNO(outArr.Ptr(), &arr[startIndex], sizeof(NN<T>) * (endIndex - startIndex));
		return endIndex - startIndex;
	}

	template <class T> UIntOS ArrayListNN<T>::RemoveRange(UIntOS Index, UIntOS cnt)
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
#if defined(_MSC_VER)
		MemCopyO(&arr[startIndex], &arr[endIndex], sizeof(NN<T>) * (objCnt - endIndex));
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

	template <class T> UnsafeArray<NN<T>> ArrayListNN<T>::GetArr(OutParam<UIntOS> arraySize) const
	{
		arraySize.Set(this->objCnt);
		return this->arr;
	}
	
	template <class T> UnsafeArray<NN<T>> ArrayListNN<T>::Arr() const
	{
		return this->arr;
	}

	template <class T> Optional<T> ArrayListNN<T>::GetLast()
	{
		if (this->objCnt == 0) return nullptr;
		return arr[this->objCnt - 1];
	}

	template <class T> Optional<T> ArrayListNN<T>::Pop()
	{
		if (this->objCnt == 0) return nullptr;
		NN<T> o = arr[this->objCnt - 1];
		this->objCnt--;
		return o;
	}

	template <class T> ArrayListNN<T> &ArrayListNN<T>::operator =(const ArrayListNN<T> &v)
	{
		this->Clear();
		this->EnsureCapacity(v.capacity);
		this->AddAll(v);
		return NNTHIS;
	}

	template <class T> void ArrayListNN<T>::DeleteAll()
	{
		UIntOS i = this->objCnt;
		while (i-- > 0)
		{
			this->arr[i].Delete();
		}
		this->objCnt = 0;
	}

	template <class T> void ArrayListNN<T>::FreeAll(FreeFunc freeFunc)
	{
		UIntOS i = this->objCnt;
		while (i-- > 0)
		{
			freeFunc(this->arr[i]);
		}
		this->objCnt = 0;
	}

	template <class T> void ArrayListNN<T>::MemFreeAll()
	{
		UIntOS i = this->objCnt;
		while (i-- > 0)
		{
			MemFreeNN(this->arr[i]);
		}
		this->objCnt = 0;
	}

	template <class T> Data::DataArray<NN<T>> ArrayListNN<T>::ToArray() const
	{
		return Data::DataArray<NN<T>>(this->arr, this->objCnt);
	}

	template <class T> void ArrayListNN<T>::Reverse()
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
