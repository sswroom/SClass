#ifndef _SM_DATA_ARRAYMAP
#define _SM_DATA_ARRAYMAP
#include "Data/ListMap.h"
#include "Data/SortableArrayListNative.h"

namespace Data
{
	template <class T, class V> class ArrayMap : public ListMap<T, V>
	{
	protected:
		Data::SortableArrayListNative<T> *keys;
		Data::ArrayList<V> vals;

	public:
		ArrayMap();
		virtual ~ArrayMap();

		virtual V Put(T key, V val);
		virtual V Get(T key) const;
		virtual V Remove(T key);
		virtual T GetKey(UOSInt index) const;
		OSInt GetIndex(T key) const;
		Bool ContainsKey(T key) const;

		void AllocSize(UOSInt cnt);
		const Data::ArrayList<V> *GetValues() const;
		const Data::SortableArrayListNative<T> *GetKeys() const;
		virtual UOSInt GetCount() const;
		virtual V GetItem(UOSInt index) const;
		virtual Bool IsEmpty() const;
		virtual V *ToArray(UOSInt *objCnt);
		virtual void Clear();
	};


	template <class T, class V> ArrayMap<T, V>::ArrayMap() : ListMap<T, V>()
	{
	}

	template <class T, class V> ArrayMap<T, V>::~ArrayMap()
	{
	}

	template <class T, class V> V ArrayMap<T, V>::Put(T key, V val)
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

	template <class T, class V> V ArrayMap<T, V>::Get(T key) const
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

	template <class T, class V> V ArrayMap<T, V>::Remove(T key)
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

	template <class T, class V> T ArrayMap<T, V>::GetKey(UOSInt index) const
	{
		return this->keys->GetItem(index);
	}

	template <class T, class V> OSInt ArrayMap<T, V>::GetIndex(T key) const
	{
		return this->keys->SortedIndexOf(key);
	}

	template <class T, class V> Bool ArrayMap<T, V>::ContainsKey(T key) const
	{
		return this->keys->SortedIndexOf(key) >= 0;
	}

	template <class T, class V> void ArrayMap<T, V>::AllocSize(UOSInt cnt)
	{
		UOSInt newSize = this->keys->GetCount() + cnt;
		this->keys->EnsureCapacity(newSize);
		this->vals.EnsureCapacity(newSize);
	}

	template <class T, class V> const Data::ArrayList<V> *ArrayMap<T, V>::GetValues() const
	{
		return &this->vals;
	}

	template <class T, class V> const Data::SortableArrayListNative<T> *ArrayMap<T, V>::GetKeys() const
	{
		return this->keys;
	}

	template <class T, class V> UOSInt ArrayMap<T, V>::GetCount() const
	{
		return this->vals.GetCount();
	}

	template <class T, class V> V ArrayMap<T, V>::GetItem(UOSInt index) const
	{
		return this->vals.GetItem(index);
	}

	template <class T, class V> Bool ArrayMap<T, V>::IsEmpty() const
	{
		return this->vals.GetCount() == 0;
	}

	template <class T, class V> V *ArrayMap<T, V>::ToArray(UOSInt *objCnt)
	{
		UOSInt cnt;
		V *arr = this->vals.GetArray(&cnt);
		V *outArr = MemAlloc(V, cnt);
		MemCopyNO(outArr, arr, sizeof(V) * cnt);
		*objCnt = cnt;
		return outArr;
	}

	template <class T, class V> void ArrayMap<T, V>::Clear()
	{
		this->keys->Clear();
		this->vals.Clear();
	}
}

#endif
