#ifndef _SM_DATA_ARRAYCMPMAPNN
#define _SM_DATA_ARRAYCMPMAPNN
#include "Data/ArrayListNN.h"
#include "Data/ListMapNN.h"
#include "Data/SortableArrayList.h"

namespace Data
{
	template <class T, class V> class ArrayCmpMapNN : public ListMapNN<T, V>
	{
	protected:
		Data::SortableArrayList<T> *keys;
		Data::ArrayListNN<V> vals;

		ArrayCmpMapNN();
	public:
		virtual ~ArrayCmpMapNN();

		virtual Optional<V> Put(T key, NN<V> val);
		virtual Optional<V> Get(T key) const;
		virtual Optional<V> Remove(T key);
		virtual T GetKey(UOSInt index) const;
		OSInt GetIndex(T key) const;
		Bool ContainsKey(T key) const;

		void AllocSize(UOSInt cnt);
		NN<const Data::ArrayListNN<V>> GetValues() const;
		NN<Data::SortableArrayList<T>> GetKeys() const;
		virtual UOSInt GetCount() const;
		virtual Optional<V> GetItem(UOSInt index) const;
		virtual NN<V> GetItemNoCheck(UOSInt index) const;
		virtual Bool IsEmpty() const;
		virtual NN<V> *ToArray(OutParam<UOSInt> objCnt);
		virtual void Clear();
	};


	template <class T, class V> ArrayCmpMapNN<T, V>::ArrayCmpMapNN() : ListMapNN<T, V>()
	{
		this->keys = 0;
	}

	template <class T, class V> ArrayCmpMapNN<T, V>::~ArrayCmpMapNN()
	{
	}

	template <class T, class V> Optional<V> ArrayCmpMapNN<T, V>::Put(T key, NN<V> val)
	{
		OSInt i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			Optional<V> oldVal = this->vals.GetItem((UOSInt)i);
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

	template <class T, class V> Optional<V> ArrayCmpMapNN<T, V>::Get(T key) const
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

	template <class T, class V> Optional<V> ArrayCmpMapNN<T, V>::Remove(T key)
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

	template <class T, class V> T ArrayCmpMapNN<T, V>::GetKey(UOSInt index) const
	{
		return this->keys->GetItem(index);
	}

	template <class T, class V> OSInt ArrayCmpMapNN<T, V>::GetIndex(T key) const
	{
		return this->keys->SortedIndexOf(key);
	}

	template <class T, class V> Bool ArrayCmpMapNN<T, V>::ContainsKey(T key) const
	{
		return this->keys->SortedIndexOf(key) >= 0;
	}

	template <class T, class V> void ArrayCmpMapNN<T, V>::AllocSize(UOSInt cnt)
	{
		UOSInt newSize = this->keys->GetCount() + cnt;
		this->keys->EnsureCapacity(newSize);
		this->vals.EnsureCapacity(newSize);
	}

	template <class T, class V> NN<const Data::ArrayListNN<V>> ArrayCmpMapNN<T, V>::GetValues() const
	{
		return this->vals;
	}

	template <class T, class V> NN<Data::SortableArrayList<T>> ArrayCmpMapNN<T, V>::GetKeys() const
	{
		return NN<Data::SortableArrayList<T>>::FromPtr(this->keys);
	}

	template <class T, class V> UOSInt ArrayCmpMapNN<T, V>::GetCount() const
	{
		return this->vals.GetCount();
	}

	template <class T, class V> Optional<V> ArrayCmpMapNN<T, V>::GetItem(UOSInt index) const
	{
		return this->vals.GetItem(index);
	}

	template <class T, class V> NN<V> ArrayCmpMapNN<T, V>::GetItemNoCheck(UOSInt index) const
	{
		return this->vals.GetItemNoCheck(index);
	}

	template <class T, class V> Bool ArrayCmpMapNN<T, V>::IsEmpty() const
	{
		return this->vals.GetCount() == 0;
	}

	template <class T, class V> NN<V> *ArrayCmpMapNN<T, V>::ToArray(OutParam<UOSInt> objCnt)
	{
		UOSInt cnt;
		UnsafeArray<NN<V>> arr = this->vals.GetPtr(cnt);
		NN<V> *outArr = MemAlloc(NN<V>, cnt);
		MemCopyNO(outArr, arr.Ptr(), sizeof(NN<V>) * cnt);
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
