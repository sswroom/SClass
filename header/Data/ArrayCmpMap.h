#ifndef _SM_DATA_ARRAYCMPMAP
#define _SM_DATA_ARRAYCMPMAP
#include "Data/ListMap.h"
#include "Data/SortableArrayList.h"

namespace Data
{
	template <class T, class V> class ArrayCmpMap : public ListMap<T, V>
	{
	protected:
		Data::SortableArrayList<T> *keys;
		Data::ArrayList<V> vals;

		ArrayCmpMap();
	public:
		virtual ~ArrayCmpMap();

		virtual V Put(T key, V val);
		virtual V Get(T key) const;
		virtual V Remove(T key);
		virtual T GetKey(UOSInt index) const;
		OSInt GetIndex(T key) const;
		Bool ContainsKey(T key) const;

		void AllocSize(UOSInt cnt);
		const Data::ArrayList<V> *GetValues() const;
		Data::SortableArrayList<T> *GetKeys() const;
		virtual UOSInt GetCount() const;
		virtual V GetItem(UOSInt index) const;
		virtual Bool IsEmpty() const;
		virtual V *ToArray(UOSInt *objCnt);
		virtual void Clear();
	};


	template <class T, class V> ArrayCmpMap<T, V>::ArrayCmpMap() : ListMap<T, V>()
	{
	}

	template <class T, class V> ArrayCmpMap<T, V>::~ArrayCmpMap()
	{
	}

	template <class T, class V> V ArrayCmpMap<T, V>::Put(T key, V val)
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

	template <class T, class V> V ArrayCmpMap<T, V>::Get(T key) const
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

	template <class T, class V> V ArrayCmpMap<T, V>::Remove(T key)
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

	template <class T, class V> T ArrayCmpMap<T, V>::GetKey(UOSInt index) const
	{
		return this->keys->GetItem(index);
	}

	template <class T, class V> OSInt ArrayCmpMap<T, V>::GetIndex(T key) const
	{
		return this->keys->SortedIndexOf(key);
	}

	template <class T, class V> Bool ArrayCmpMap<T, V>::ContainsKey(T key) const
	{
		return this->keys->SortedIndexOf(key) >= 0;
	}

	template <class T, class V> void ArrayCmpMap<T, V>::AllocSize(UOSInt cnt)
	{
		UOSInt newSize = this->keys->GetCount() + cnt;
		this->keys->EnsureCapacity(newSize);
		this->vals.EnsureCapacity(newSize);
	}

	template <class T, class V> const Data::ArrayList<V> *ArrayCmpMap<T, V>::GetValues() const
	{
		return &this->vals;
	}

	template <class T, class V> Data::SortableArrayList<T> *ArrayCmpMap<T, V>::GetKeys() const
	{
		return this->keys;
	}

	template <class T, class V> UOSInt ArrayCmpMap<T, V>::GetCount() const
	{
		return this->vals.GetCount();
	}

	template <class T, class V> V ArrayCmpMap<T, V>::GetItem(UOSInt index) const
	{
		return this->vals.GetItem(index);
	}

	template <class T, class V> Bool ArrayCmpMap<T, V>::IsEmpty() const
	{
		return this->vals.GetCount() == 0;
	}

	template <class T, class V> V *ArrayCmpMap<T, V>::ToArray(UOSInt *objCnt)
	{
		UOSInt cnt;
		V *arr = this->vals.GetArray(&cnt);
		V *outArr = MemAlloc(V, cnt);
		MemCopyNO(outArr, arr, sizeof(V) * cnt);
		*objCnt = cnt;
		return outArr;
	}

	template <class T, class V> void ArrayCmpMap<T, V>::Clear()
	{
		this->keys->Clear();
		this->vals.Clear();
	}
}

#endif
