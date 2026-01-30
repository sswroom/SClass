#ifndef _SM_DATA_ARRAYCMPMAPOBJ
#define _SM_DATA_ARRAYCMPMAPOBJ
#include "Data/ListMap.hpp"
#include "Data/SortableArrayListObj.hpp"

namespace Data
{
	template <class T, class V> class ArrayCmpMapObj : public ListMap<T, V>
	{
	protected:
		NN<Data::SortableArrayListObj<T>> keys;
		Data::ArrayListObj<V> vals;

		ArrayCmpMapObj(NN<Data::SortableArrayListObj<T>> keys);
	public:
		virtual ~ArrayCmpMapObj();

		virtual V Put(T key, V val);
		virtual V Get(T key) const;
		virtual V Remove(T key);
		virtual T GetKey(UIntOS index) const;
		IntOS GetIndex(T key) const;
		Bool ContainsKey(T key) const;

		void AllocSize(UIntOS cnt);
		NN<const Data::ArrayListObj<V>> GetValues() const;
		NN<Data::SortableArrayListObj<T>> GetKeys() const;
		virtual UIntOS GetCount() const;
		virtual V GetItem(UIntOS index) const;
		virtual Bool IsEmpty() const;
		virtual UnsafeArray<V> ToArray(OutParam<UIntOS> objCnt);
		virtual void Clear();
	};


	template <class T, class V> ArrayCmpMapObj<T, V>::ArrayCmpMapObj(NN<Data::SortableArrayListObj<T>> keys) : ListMap<T, V>()
	{
		this->keys = keys;
	}

	template <class T, class V> ArrayCmpMapObj<T, V>::~ArrayCmpMapObj()
	{
	}

	template <class T, class V> V ArrayCmpMapObj<T, V>::Put(T key, V val)
	{
		IntOS i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			V oldVal = this->vals.GetItem((UIntOS)i);
            this->vals.SetItem((UIntOS)i, val);
			return oldVal;
		}
		else
		{
			this->keys->Insert((UIntOS)~i, key);
			this->vals.Insert((UIntOS)~i, val);
			return nullptr;
		}
	}

	template <class T, class V> V ArrayCmpMapObj<T, V>::Get(T key) const
	{
		IntOS i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			return this->vals.GetItem((UIntOS)i);
		}
		else
		{
			return nullptr;
		}
	}

	template <class T, class V> V ArrayCmpMapObj<T, V>::Remove(T key)
	{
		IntOS i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			this->keys->RemoveAt((UIntOS)i);
			return this->vals.RemoveAt((UIntOS)i);
		}
		else
		{
			return nullptr;
		}
	}

	template <class T, class V> T ArrayCmpMapObj<T, V>::GetKey(UIntOS index) const
	{
		return this->keys->GetItem(index);
	}

	template <class T, class V> IntOS ArrayCmpMapObj<T, V>::GetIndex(T key) const
	{
		return this->keys->SortedIndexOf(key);
	}

	template <class T, class V> Bool ArrayCmpMapObj<T, V>::ContainsKey(T key) const
	{
		return this->keys->SortedIndexOf(key) >= 0;
	}

	template <class T, class V> void ArrayCmpMapObj<T, V>::AllocSize(UIntOS cnt)
	{
		UIntOS newSize = this->keys->GetCount() + cnt;
		this->keys->EnsureCapacity(newSize);
		this->vals.EnsureCapacity(newSize);
	}

	template <class T, class V> NN<const Data::ArrayListObj<V>> ArrayCmpMapObj<T, V>::GetValues() const
	{
		return this->vals;
	}

	template <class T, class V> NN<Data::SortableArrayListObj<T>> ArrayCmpMapObj<T, V>::GetKeys() const
	{
		return this->keys;
	}

	template <class T, class V> UIntOS ArrayCmpMapObj<T, V>::GetCount() const
	{
		return this->vals.GetCount();
	}

	template <class T, class V> V ArrayCmpMapObj<T, V>::GetItem(UIntOS index) const
	{
		return this->vals.GetItem(index);
	}

	template <class T, class V> Bool ArrayCmpMapObj<T, V>::IsEmpty() const
	{
		return this->vals.GetCount() == 0;
	}

	template <class T, class V> UnsafeArray<V> ArrayCmpMapObj<T, V>::ToArray(OutParam<UIntOS> objCnt)
	{
		UIntOS cnt;
		UnsafeArray<V> arr = this->vals.GetArr(cnt);
		UnsafeArray<V> outArr = MemAlloc(V, cnt);
		outArr.CopyFromNO(arr, cnt);
		objCnt.Set(cnt);
		return outArr;
	}

	template <class T, class V> void ArrayCmpMapObj<T, V>::Clear()
	{
		this->keys->Clear();
		this->vals.Clear();
	}
}

#endif
