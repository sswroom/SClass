#ifndef _SM_DATA_ARRAYCMPMAPNATIVE
#define _SM_DATA_ARRAYCMPMAPNATIVE
#include "Data/ArrayListNative.hpp"
#include "Data/ListMap.hpp"
#include "Data/SortableArrayListObj.hpp"

namespace Data
{
	template <class T, class V> class ArrayCmpMapNative : public ListMap<T, V>
	{
	protected:
		NN<Data::SortableArrayListObj<T>> keys;
		Data::ArrayListNative<V> vals;

		ArrayCmpMapNative(NN<Data::SortableArrayListObj<T>> keys);
	public:
		virtual ~ArrayCmpMapNative();

		virtual V Put(T key, V val);
		virtual V Get(T key) const;
		virtual V Remove(T key);
		virtual T GetKey(UOSInt index) const;
		OSInt GetIndex(T key) const;
		Bool ContainsKey(T key) const;

		void AllocSize(UOSInt cnt);
		NN<const Data::ArrayListNative<V>> GetValues() const;
		NN<Data::SortableArrayListObj<T>> GetKeys() const;
		virtual UOSInt GetCount() const;
		virtual V GetItem(UOSInt index) const;
		virtual Bool IsEmpty() const;
		virtual UnsafeArray<V> ToArray(OutParam<UOSInt> objCnt);
		virtual void Clear();
	};


	template <class T, class V> ArrayCmpMapNative<T, V>::ArrayCmpMapNative(NN<Data::SortableArrayListObj<T>> keys) : ListMap<T, V>()
	{
		this->keys = keys;
	}

	template <class T, class V> ArrayCmpMapNative<T, V>::~ArrayCmpMapNative()
	{
	}

	template <class T, class V> V ArrayCmpMapNative<T, V>::Put(T key, V val)
	{
		OSInt i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			V oldVal = this->vals.GetItem((UOSInt)i);
            this->vals.SetItem((UOSInt)i, val);
			return oldVal;
		}
		else
		{
			this->keys->Insert((UOSInt)~i, key);
			this->vals.Insert((UOSInt)~i, val);
			return 0;
		}
	}

	template <class T, class V> V ArrayCmpMapNative<T, V>::Get(T key) const
	{
		OSInt i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			return this->vals.GetItem((UOSInt)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T, class V> V ArrayCmpMapNative<T, V>::Remove(T key)
	{
		OSInt i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			this->keys->RemoveAt((UOSInt)i);
			return this->vals.RemoveAt((UOSInt)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T, class V> T ArrayCmpMapNative<T, V>::GetKey(UOSInt index) const
	{
		return this->keys->GetItem(index);
	}

	template <class T, class V> OSInt ArrayCmpMapNative<T, V>::GetIndex(T key) const
	{
		return this->keys->SortedIndexOf(key);
	}

	template <class T, class V> Bool ArrayCmpMapNative<T, V>::ContainsKey(T key) const
	{
		return this->keys->SortedIndexOf(key) >= 0;
	}

	template <class T, class V> void ArrayCmpMapNative<T, V>::AllocSize(UOSInt cnt)
	{
		UOSInt newSize = this->keys->GetCount() + cnt;
		this->keys->EnsureCapacity(newSize);
		this->vals.EnsureCapacity(newSize);
	}

	template <class T, class V> NN<const Data::ArrayListNative<V>> ArrayCmpMapNative<T, V>::GetValues() const
	{
		return this->vals;
	}

	template <class T, class V> NN<Data::SortableArrayListObj<T>> ArrayCmpMapNative<T, V>::GetKeys() const
	{
		return this->keys;
	}

	template <class T, class V> UOSInt ArrayCmpMapNative<T, V>::GetCount() const
	{
		return this->vals.GetCount();
	}

	template <class T, class V> V ArrayCmpMapNative<T, V>::GetItem(UOSInt index) const
	{
		return this->vals.GetItem(index);
	}

	template <class T, class V> Bool ArrayCmpMapNative<T, V>::IsEmpty() const
	{
		return this->vals.GetCount() == 0;
	}

	template <class T, class V> UnsafeArray<V> ArrayCmpMapNative<T, V>::ToArray(OutParam<UOSInt> objCnt)
	{
		UOSInt cnt;
		UnsafeArray<V> arr = this->vals.GetArr(cnt);
		UnsafeArray<V> outArr = MemAlloc(V, cnt);
		MemCopyNO(outArr.Ptr(), arr.Ptr(), sizeof(V) * cnt);
		objCnt.Set(cnt);
		return outArr;
	}

	template <class T, class V> void ArrayCmpMapNative<T, V>::Clear()
	{
		this->keys->Clear();
		this->vals.Clear();
	}
}

#endif
