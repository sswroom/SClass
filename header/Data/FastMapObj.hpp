#ifndef _SM_DATA_FASTMAPOBJ
#define _SM_DATA_FASTMAPOBJ
#include "Data/ArrayListNative.hpp"
#include "Data/ListMap.hpp"
#include "Data/TwinItemObj.hpp"

namespace Data
{
	template <class T, class V> class FastMapObj : public ListMap<T, V>
	{
	protected:
		Data::ArrayListNative<TwinItemObj<T, V>> values;

	public:
		FastMapObj();
		virtual ~FastMapObj();

		virtual V Put(T key, V val);
		virtual V Get(T key) const;
		virtual V Remove(T key);
		OSInt GetIndex(T key) const;
		Bool ContainsKey(T key) const;

		void AllocSize(UOSInt cnt);
		UOSInt AddKeysTo(NN<List<T>> list);
		virtual UOSInt GetCount() const;
		virtual T GetKey(UOSInt index) const;
		virtual V GetItem(UOSInt index) const;
		V RemoveAt(UOSInt index);
		virtual Bool IsEmpty() const;
		virtual UnsafeArray<V> ToArray(OutParam<UOSInt> objCnt);
		virtual void Clear();
	};


	template <class T, class V> FastMapObj<T, V>::FastMapObj() : ListMap<T, V>()
	{
	}

	template <class T, class V> FastMapObj<T, V>::~FastMapObj()
	{
	}

	template <class T, class V> V FastMapObj<T, V>::Put(T key, V val)
	{
		OSInt i;
		i = this->GetIndex(key);
		if (i >= 0)
		{
			V oldVal = this->values.GetItem((UOSInt)i).value;
            this->values.SetItem((UOSInt)i, TwinItemObj<T,V>(key, val));
			return oldVal;
		}
		else
		{
			this->values.Insert((UOSInt)~i, TwinItemObj<T,V>(key, val));
			return nullptr;
		}
	}

	template <class T, class V> V FastMapObj<T, V>::Get(T key) const
	{
		OSInt i;
		i = this->GetIndex(key);
		if (i >= 0)
		{
			return this->values.GetItem((UOSInt)i).value;
		}
		else
		{
			return nullptr;
		}
	}

	template <class T, class V> V FastMapObj<T, V>::Remove(T key)
	{
		OSInt i;
		i = this->GetIndex(key);
		if (i >= 0)
		{
			return this->values.RemoveAt((UOSInt)i).value;
		}
		else
		{
			return nullptr;
		}
	}

	template <class T, class V> OSInt FastMapObj<T, V>::GetIndex(T key) const
	{
		OSInt i;
		OSInt j;
		OSInt k;
		T l;
		i = 0;
		j = (OSInt)this->values.GetCount() - 1;
		while (i <= j)
		{
			k = (i + j) >> 1;
			l = this->values.GetItem((UOSInt)k).key;
			if (l > key)
			{
				j = k - 1;
			}
			else if (l < key)
			{
				i = k + 1;
			}
			else
			{
				return k;
			}
		}
		return -i - 1;
	}

	template <class T, class V> Bool FastMapObj<T, V>::ContainsKey(T key) const
	{
		return this->GetIndex(key) >= 0;
	}

	template <class T, class V> void FastMapObj<T, V>::AllocSize(UOSInt cnt)
	{
		UOSInt newSize = this->values.GetCount() + cnt;
		this->values.EnsureCapacity(newSize);
	}

	template <class T, class V> UOSInt FastMapObj<T, V>::AddKeysTo(NN<List<T>> list)
	{
		UOSInt i = 0;
		UOSInt j = this->values.GetCount();
		while (i < j)
		{
			list->Add(this->values.GetItem(i).key);
			i++;
		}
		return j;
	}

	template <class T, class V> UOSInt FastMapObj<T, V>::GetCount() const
	{
		return this->values.GetCount();
	}

	template <class T, class V> T FastMapObj<T, V>::GetKey(UOSInt index) const
	{
		return this->values.GetItem(index).key;
	}

	template <class T, class V> V FastMapObj<T, V>::GetItem(UOSInt index) const
	{
		return this->values.GetItem(index).value;
	}

	template <class T, class V> V FastMapObj<T, V>::RemoveAt(UOSInt index)
	{
		return this->values.RemoveAt(index).value;
	}
	
	template <class T, class V> Bool FastMapObj<T, V>::IsEmpty() const
	{
		return this->values.GetCount() == 0;
	}

	template <class T, class V> UnsafeArray<V> FastMapObj<T, V>::ToArray(OutParam<UOSInt> objCnt)
	{
		UOSInt cnt = this->values.GetCount();
		UnsafeArray<V> outArr = MemAllocArr(V, cnt);
		objCnt.Set(cnt);
		while (cnt-- > 0)
		{
			outArr[cnt] = this->values.GetItem(cnt).value;
		}
		return outArr;
	}

	template <class T, class V> void FastMapObj<T, V>::Clear()
	{
		this->values.Clear();
	}

	template <class V> class Int32FastMapObj : public FastMapObj<Int32, V>
	{

	};

	template <class V> class UInt32FastMapObj : public FastMapObj<UInt32, V>
	{

	};

	template <class V> class Int64FastMapObj : public FastMapObj<Int64, V>
	{

	};

	template <class V> class UInt64FastMapObj : public FastMapObj<UInt64, V>
	{

	};
}

#endif
