#ifndef _SM_DATA_ARRAYCMPMAPNN
#define _SM_DATA_ARRAYCMPMAPNN
#include "Data/ArrayListNN.hpp"
#include "Data/ListMapNN.hpp"
#include "Data/SortableArrayListObj.hpp"

namespace Data
{
	template <class T, class V> class ArrayCmpMapNN : public ListMapNN<T, V>
	{
	protected:
		NN<Data::SortableArrayListObj<T>> keys;
		Data::ArrayListNN<V> vals;

		ArrayCmpMapNN(NN<Data::SortableArrayListObj<T>> keys);
	public:
		virtual ~ArrayCmpMapNN();

		virtual Optional<V> Put(T key, NN<V> val);
		virtual Optional<V> Get(T key) const;
		virtual Optional<V> Remove(T key);
		virtual T GetKey(UIntOS index) const;
		IntOS GetIndex(T key) const;
		Bool ContainsKey(T key) const;

		void AllocSize(UIntOS cnt);
		NN<const Data::ArrayListNN<V>> GetValues() const;
		NN<Data::SortableArrayListObj<T>> GetKeys() const;
		virtual UIntOS GetCount() const;
		virtual Optional<V> GetItem(UIntOS index) const;
		virtual NN<V> GetItemNoCheck(UIntOS index) const;
		virtual Bool IsEmpty() const;
		virtual UnsafeArray<NN<V>> ToArray(OutParam<UIntOS> objCnt);
		virtual void Clear();
	};


	template <class T, class V> ArrayCmpMapNN<T, V>::ArrayCmpMapNN(NN<Data::SortableArrayListObj<T>> keys) : ListMapNN<T, V>()
	{
		this->keys = keys;
	}

	template <class T, class V> ArrayCmpMapNN<T, V>::~ArrayCmpMapNN()
	{
	}

	template <class T, class V> Optional<V> ArrayCmpMapNN<T, V>::Put(T key, NN<V> val)
	{
		IntOS i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			Optional<V> oldVal = this->vals.GetItem((UIntOS)i);
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

	template <class T, class V> Optional<V> ArrayCmpMapNN<T, V>::Get(T key) const
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

	template <class T, class V> Optional<V> ArrayCmpMapNN<T, V>::Remove(T key)
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

	template <class T, class V> T ArrayCmpMapNN<T, V>::GetKey(UIntOS index) const
	{
		return this->keys->GetItem(index);
	}

	template <class T, class V> IntOS ArrayCmpMapNN<T, V>::GetIndex(T key) const
	{
		return this->keys->SortedIndexOf(key);
	}

	template <class T, class V> Bool ArrayCmpMapNN<T, V>::ContainsKey(T key) const
	{
		return this->keys->SortedIndexOf(key) >= 0;
	}

	template <class T, class V> void ArrayCmpMapNN<T, V>::AllocSize(UIntOS cnt)
	{
		UIntOS newSize = this->keys->GetCount() + cnt;
		this->keys->EnsureCapacity(newSize);
		this->vals.EnsureCapacity(newSize);
	}

	template <class T, class V> NN<const Data::ArrayListNN<V>> ArrayCmpMapNN<T, V>::GetValues() const
	{
		return this->vals;
	}

	template <class T, class V> NN<Data::SortableArrayListObj<T>> ArrayCmpMapNN<T, V>::GetKeys() const
	{
		return this->keys;
	}

	template <class T, class V> UIntOS ArrayCmpMapNN<T, V>::GetCount() const
	{
		return this->vals.GetCount();
	}

	template <class T, class V> Optional<V> ArrayCmpMapNN<T, V>::GetItem(UIntOS index) const
	{
		return this->vals.GetItem(index);
	}

	template <class T, class V> NN<V> ArrayCmpMapNN<T, V>::GetItemNoCheck(UIntOS index) const
	{
		return this->vals.GetItemNoCheck(index);
	}

	template <class T, class V> Bool ArrayCmpMapNN<T, V>::IsEmpty() const
	{
		return this->vals.GetCount() == 0;
	}

	template <class T, class V> UnsafeArray<NN<V>> ArrayCmpMapNN<T, V>::ToArray(OutParam<UIntOS> objCnt)
	{
		UIntOS cnt;
		UnsafeArray<NN<V>> arr = this->vals.GetArr(cnt);
		UnsafeArray<NN<V>> outArr = MemAllocArr(NN<V>, cnt);
		MemCopyNO(outArr.Ptr(), arr.Ptr(), sizeof(NN<V>) * cnt);
		objCnt.Set(cnt);
		return outArr;
	}

	template <class T, class V> void ArrayCmpMapNN<T, V>::Clear()
	{
		this->keys->Clear();
		this->vals.Clear();
	}
}

#endif
