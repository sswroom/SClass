#ifndef _SM_DATA_ARRAYCMPMAP
#define _SM_DATA_ARRAYCMPMAP
#include "Data/IMap.h"
#include "Data/SortableArrayList.h"

namespace Data
{
	template <class T, class V> class ArrayCmpMap : public IMap<T, V>
	{
	protected:
		Data::SortableArrayList<T> *keys;
		Data::ArrayList<V> *vals;

	public:
		ArrayCmpMap();
		virtual ~ArrayCmpMap();

		virtual V Put(T key, V val);
		virtual V Get(T key);
		virtual V Remove(T key);
		T GetKey(UOSInt index);
		void PutAll(ArrayCmpMap<T,V> *map);
		OSInt GetIndex(T key);
		Bool ContainsKey(T key);

		void AllocSize(UOSInt cnt);
		Data::ArrayList<V> *GetValues();
		Data::SortableArrayList<T> *GetKeys();
		UOSInt GetCount();
		virtual Bool IsEmpty();
		virtual V *ToArray(UOSInt *objCnt);
		virtual void Clear();
	};


	template <class T, class V> ArrayCmpMap<T, V>::ArrayCmpMap() : IMap<T, V>()
	{
		NEW_CLASS(vals, Data::ArrayList<V>());
	}

	template <class T, class V> ArrayCmpMap<T, V>::~ArrayCmpMap()
	{
		DEL_CLASS(vals);
	}

	template <class T, class V> V ArrayCmpMap<T, V>::Put(T key, V val)
	{
		OSInt i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			V oldVal = this->vals->GetItem((UOSInt)i);
            this->vals->SetItem((UOSInt)i, val);
			return oldVal;
		}
		else
		{
			this->keys->Insert((UOSInt)~i, key);
			this->vals->Insert((UOSInt)~i, val);
			return 0;
		}
	}

	template <class T, class V> V ArrayCmpMap<T, V>::Get(T key)
	{
		OSInt i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			return this->vals->GetItem((UOSInt)i);
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
			return this->vals->RemoveAt((UOSInt)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T, class V> T ArrayCmpMap<T, V>::GetKey(UOSInt index)
	{
		return this->keys->GetItem(index);
	}

	template <class T, class V> void ArrayCmpMap<T, V>::PutAll(ArrayCmpMap<T,V> *map)
	{
		Data::ArrayList<T> *tList = map->GetKeys();
		Data::ArrayList<V> *vList = map->GetValues();
		UOSInt i;
		UOSInt j;
		i = 0;
		j = tList->GetCount();
		while (i < j)
		{
			this->Put(tList->GetItem(i), vList->GetItem(i));
			i++;
		}
	}

	template <class T, class V> OSInt ArrayCmpMap<T, V>::GetIndex(T key)
	{
		return this->keys->SortedIndexOf(key);
	}

	template <class T, class V> Bool ArrayCmpMap<T, V>::ContainsKey(T key)
	{
		return this->keys->SortedIndexOf(key) >= 0;
	}

	template <class T, class V> void ArrayCmpMap<T, V>::AllocSize(UOSInt cnt)
	{
		UOSInt newSize = this->keys->GetCount() + cnt;
		this->keys->EnsureCapacity(newSize);
		this->vals->EnsureCapacity(newSize);
	}

	template <class T, class V> Data::ArrayList<V> *ArrayCmpMap<T, V>::GetValues()
	{
		return this->vals;
	}

	template <class T, class V> Data::SortableArrayList<T> *ArrayCmpMap<T, V>::GetKeys()
	{
		return this->keys;
	}

	template <class T, class V> UOSInt ArrayCmpMap<T, V>::GetCount()
	{
		return this->vals->GetCount();
	}

	template <class T, class V> Bool ArrayCmpMap<T, V>::IsEmpty()
	{
		return this->vals->GetCount() == 0;
	}

	template <class T, class V> V *ArrayCmpMap<T, V>::ToArray(UOSInt *objCnt)
	{
		UOSInt cnt;
		V *arr = this->vals->GetArray(&cnt);
		V *outArr = MemAlloc(V, cnt);
		MemCopyNO(outArr, arr, sizeof(V) * cnt);
		*objCnt = cnt;
		return outArr;
	}

	template <class T, class V> void ArrayCmpMap<T, V>::Clear()
	{
		this->keys->Clear();
		this->vals->Clear();
	}
}

#endif