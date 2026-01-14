#ifndef _SM_DATA_FASTMAPNATIVE
#define _SM_DATA_FASTMAPNATIVE
#include "Data/ArrayListNative.hpp"
#include "Data/ListMap.hpp"
#include "Data/TwinItemNative.hpp"

namespace Data
{
	template <class T, class V> class FastMapNative : public ListMap<T, V>
	{
	protected:
		Data::ArrayListNative<TwinItemNative<T, V>> values;

	public:
		FastMapNative();
		virtual ~FastMapNative();

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


	template <class T, class V> FastMapNative<T, V>::FastMapNative() : ListMap<T, V>()
	{
	}

	template <class T, class V> FastMapNative<T, V>::~FastMapNative()
	{
	}

	template <class T, class V> V FastMapNative<T, V>::Put(T key, V val)
	{
		OSInt i;
		i = this->GetIndex(key);
		if (i >= 0)
		{
			V oldVal = this->values.GetItem((UOSInt)i).value;
            this->values.SetItem((UOSInt)i, TwinItemNative<T,V>(key, val));
			return oldVal;
		}
		else
		{
			this->values.Insert((UOSInt)~i, TwinItemNative<T,V>(key, val));
			return 0;
		}
	}

	template <class T, class V> V FastMapNative<T, V>::Get(T key) const
	{
		OSInt i;
		i = this->GetIndex(key);
		if (i >= 0)
		{
			return this->values.GetItem((UOSInt)i).value;
		}
		else
		{
			return 0;
		}
	}

	template <class T, class V> V FastMapNative<T, V>::Remove(T key)
	{
		OSInt i;
		i = this->GetIndex(key);
		if (i >= 0)
		{
			return this->values.RemoveAt((UOSInt)i).value;
		}
		else
		{
			return 0;
		}
	}

	template <class T, class V> OSInt FastMapNative<T, V>::GetIndex(T key) const
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

	template <class T, class V> Bool FastMapNative<T, V>::ContainsKey(T key) const
	{
		return this->GetIndex(key) >= 0;
	}

	template <class T, class V> void FastMapNative<T, V>::AllocSize(UOSInt cnt)
	{
		UOSInt newSize = this->values.GetCount() + cnt;
		this->values.EnsureCapacity(newSize);
	}

	template <class T, class V> UOSInt FastMapNative<T, V>::AddKeysTo(NN<List<T>> list)
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

	template <class T, class V> UOSInt FastMapNative<T, V>::GetCount() const
	{
		return this->values.GetCount();
	}

	template <class T, class V> T FastMapNative<T, V>::GetKey(UOSInt index) const
	{
		return this->values.GetItem(index).key;
	}

	template <class T, class V> V FastMapNative<T, V>::GetItem(UOSInt index) const
	{
		return this->values.GetItem(index).value;
	}

	template <class T, class V> V FastMapNative<T, V>::RemoveAt(UOSInt index)
	{
		return this->values.RemoveAt(index).value;
	}
	
	template <class T, class V> Bool FastMapNative<T, V>::IsEmpty() const
	{
		return this->values.GetCount() == 0;
	}

	template <class T, class V> UnsafeArray<V> FastMapNative<T, V>::ToArray(OutParam<UOSInt> objCnt)
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

	template <class T, class V> void FastMapNative<T, V>::Clear()
	{
		this->values.Clear();
	}

	template <class V> class Int32FastMapNative : public FastMapNative<Int32, V>
	{

	};

	template <class V> class UInt32FastMapNative : public FastMapNative<UInt32, V>
	{

	};

	template <class V> class Int64FastMapNative : public FastMapNative<Int64, V>
	{

	};

	template <class V> class UInt64FastMapNative : public FastMapNative<UInt64, V>
	{

	};
}

#endif
