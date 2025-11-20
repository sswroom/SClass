#ifndef _SM_DATA_FASTSTRINGMAP
#define _SM_DATA_FASTSTRINGMAP
#include "MyMemory.h"
#include "Crypto/Hash/CRC32RC.h"
#include "Data/ListMap.hpp"
#include "Data/ReadingList.hpp"
#include "Text/MyString.h"
#include "Text/String.h"

namespace Data
{
	template <class T> struct FastStringItem
	{
		UInt32 hash;
		NN<Text::String> s;
		T val;
	};

	template <class T> class FastStringKeyIterator
	{
	private:
		UnsafeArray<const FastStringItem<T>> arr;
		UOSInt cnt;
	public:
		FastStringKeyIterator(UnsafeArray<const FastStringItem<T>> arr, UOSInt cnt)
		{
			this->arr = arr;
			this->cnt = cnt;
		}

		Bool HasNext() const
		{
			return this->cnt > 0;
		}

		NN<Text::String> Next()
		{
			this->cnt--;
			NN<Text::String> ret = arr[0].s;
			this->arr++;
			return ret;
		}
	};

	template <class T> class FastStringMap : public ListMap<Optional<Text::String>, T>
	{
	private:
		UOSInt capacity;
		UOSInt cnt;
		UnsafeArray<FastStringItem<T>> items;
		Crypto::Hash::CRC32RC crc;

		void Insert(UOSInt index, UInt32 hash, NN<Text::String> s, T val);
	public:
		FastStringMap();
		FastStringMap(NN<const FastStringMap<T>> map);
		virtual ~FastStringMap();

		virtual UOSInt GetCount() const;
		virtual T GetItem(UOSInt index) const;
		virtual Optional<Text::String> GetKey(UOSInt index) const;
		virtual OSInt IndexOf(UInt32 hash, UnsafeArray<const UTF8Char> s, UOSInt len) const;
		OSInt IndexOf(NN<Text::String> s) const;
		OSInt IndexOfC(Text::CStringNN s) const;

		virtual T Put(Optional<Text::String> key, T val);
		T PutNN(NN<Text::String> key, T val);
		T PutC(Text::CStringNN key, T val);
		virtual T Get(Optional<Text::String> key) const;
		T GetNN(NN<Text::String> key) const;
		T GetC(Text::CStringNN key) const;
		virtual T Remove(Optional<Text::String> key);
		T RemoveNN(NN<Text::String> key);
		T RemoveC(Text::CStringNN key);
		T RemoveAt(UOSInt index);
		virtual Bool IsEmpty() const;
		virtual void Clear();
		FastStringKeyIterator<T> KeyIterator() const;
		
		UInt32 CalcHash(UnsafeArray<const UTF8Char> s, UOSInt len) const;
	};

	template <class T> void FastStringMap<T>::Insert(UOSInt index, UInt32 hash, NN<Text::String> s, T val)
	{
		if (index > this->cnt)
		{
			index = this->cnt;
		}
		if (this->cnt == this->capacity)
		{
			this->capacity = this->capacity << 1;
			UnsafeArray<FastStringItem<T>> newItems = MemAllocArr(FastStringItem<T>, this->capacity);
			if (index > 0)
			{
				MemCopyNO(newItems.Ptr(), this->items.Ptr(), sizeof(FastStringItem<T>) * index);
			}
			newItems[index].hash = hash;
			newItems[index].s = s;
			newItems[index].val = val;
			if (index < this->cnt)
			{
				MemCopyNO(&newItems[index + 1], &this->items[index], sizeof(FastStringItem<T>) * (this->cnt - index));
			}
			MemFreeArr(this->items);
			this->items = newItems;
			this->cnt++;
		}
		else
		{
			if (index < this->cnt)
			{
				MemCopyO(&this->items[index + 1], &this->items[index], sizeof(FastStringItem<T>) * (this->cnt - index));
			}
			this->items[index].hash = hash;
			this->items[index].s = s;
			this->items[index].val = val;
			this->cnt++;
		}
	}

	template <class T> FastStringMap<T>::FastStringMap()
	{
		this->capacity = 64;
		this->cnt = 0;
		this->items = MemAllocArr(FastStringItem<T>, this->capacity);
	}

	template <class T> FastStringMap<T>::FastStringMap(NN<const FastStringMap<T>> map)
	{
		this->capacity = map->capacity;
		this->cnt = map->cnt;
		this->items = MemAllocArr(FastStringItem<T>, this->capacity);
		UOSInt i = 0;
		UOSInt j = this->cnt;
		while (i < j)
		{
			this->items[i].hash = map->items[i].hash;
			this->items[i].s = map->items[i].s->Clone();
			this->items[i].val = map->items[i].val;
			i++;
		}
	}

	template <class T> FastStringMap<T>::~FastStringMap()
	{
		UOSInt i = this->cnt;
		while (i-- > 0)
		{
			this->items[i].s->Release();
		}
		MemFreeArr(this->items);
	}

	template <class T> UOSInt FastStringMap<T>::GetCount() const
	{
		return this->cnt;
	}

	template <class T> T FastStringMap<T>::GetItem(UOSInt index) const
	{
		if (index >= this->cnt)
		{
			return 0;
		}
		return this->items[index].val;
	}

	template <class T> Optional<Text::String> FastStringMap<T>::GetKey(UOSInt index) const
	{
		if (index >= this->cnt)
		{
			return 0;
		}
		return this->items[index].s;
	}

	template <class T> OSInt FastStringMap<T>::IndexOf(UInt32 hash, UnsafeArray<const UTF8Char> s, UOSInt len) const
	{
		OSInt i;
		OSInt j;
		OSInt k;
//		OSInt l;
		i = 0;
		j = (OSInt)this->cnt - 1;
		while (i <= j)
		{
			k = (i + j) >> 1;
			if (this->items[k].hash == hash)
			{
				while (k > 0 && this->items[k - 1].hash == hash)
				{
					k--;
				}
				while (true)
				{
					if (this->items[k].s->Equals(s, len))
					{
						return k;
					}
					if (++k >= j || this->items[k].hash != hash)
					{
						return ~k;
					}
				}
/*				l = this->items[k].s->CompareToFast({s, len});
				if (l > 0)
				{
					j = k - 1;
				}
				else if (l < 0)
				{
					i = k + 1;
				}
				else
				{
					return k;
				}*/
/*				if (this->items[k].s->Equals(s, len))
				{
					return k;
				}
				i = k;
				while (i > 0 && this->items[i - 1].hash == hash)
				{
					i--;
					if (this->items[i].s->Equals(s, len))
					{
						return i;
					}
				}
				while (++k < j && this->items[k].hash == hash)
				{
					if (this->items[k].s->Equals(s, len))
					{
						return k;
					}
				}
				return ~k;*/
			}
			else if (this->items[k].hash > hash)
			{
				j = k - 1;
			}
			else
			{
				i = k + 1;
			}
		}
		return -i - 1;
	}

	template <class T> OSInt FastStringMap<T>::IndexOf(NN<Text::String> s) const
	{
		UInt32 hash = this->crc.CalcDirect(s->v, s->leng);
		return IndexOf(hash, s->v, s->leng);
	}

	template <class T> OSInt FastStringMap<T>::IndexOfC(Text::CStringNN s) const
	{
		UInt32 hash = this->crc.CalcDirect(s.v, s.leng);
		return IndexOf(hash, s.v, s.leng);
	}

	template <class T> T FastStringMap<T>::Put(Optional<Text::String> key, T val)
	{
		return PutNN(Text::String::OrEmpty(key), val);
	}

	template <class T> T FastStringMap<T>::PutNN(NN<Text::String> key, T val)
	{
		UInt32 hash = this->crc.CalcDirect(key->v, key->leng);
		OSInt index = this->IndexOf(hash, key->v, key->leng);
		if (index < 0)
		{
			this->Insert((UOSInt)~index, hash, key->Clone(), val);
			return 0;
		}
		else
		{
			T oldVal = this->items[index].val;
			this->items[index].val = val;
			return oldVal;
		}
	}

	template <class T> T FastStringMap<T>::PutC(Text::CStringNN key, T val)
	{
		UInt32 hash = this->crc.CalcDirect(key.v, key.leng);
		OSInt index = this->IndexOf(hash, key.v, key.leng);
		if (index < 0)
		{
			this->Insert((UOSInt)~index, hash, Text::String::New(key.v, key.leng), val);
			return 0;
		}
		else
		{
			T oldVal = this->items[index].val;
			this->items[index].val = val;
			return oldVal;
		}
	}

	template <class T> T FastStringMap<T>::Get(Optional<Text::String> key) const
	{
		return GetNN(Text::String::OrEmpty(key));
	}

	template <class T> T FastStringMap<T>::GetNN(NN<Text::String> key) const
	{
		UInt32 hash = this->crc.CalcDirect(key->v, key->leng);
		OSInt index = this->IndexOf(hash, key->v, key->leng);
		if (index >= 0)
		{
			return this->items[index].val;
		}
		return 0;
	}

	template <class T> T FastStringMap<T>::GetC(Text::CStringNN key) const
	{
		UInt32 hash = this->crc.CalcDirect(key.v, key.leng);
		OSInt index = this->IndexOf(hash, key.v, key.leng);
		if (index >= 0)
		{
			return this->items[index].val;
		}
		return 0;
	}

	template <class T> T FastStringMap<T>::Remove(Optional<Text::String> key)
	{
		return RemoveNN(Text::String::OrEmpty(key));
	}

	template <class T> T FastStringMap<T>::RemoveNN(NN<Text::String> key)
	{
		UInt32 hash = this->crc.CalcDirect(key->v, key->leng);
		OSInt index = this->IndexOf(hash, key->v, key->leng);
		if (index >= 0)
		{
			T oldVal = this->items[index].val;
			this->items[index].s->Release();
			this->cnt--;
			if ((UOSInt)index < this->cnt)
			{
				MemCopyO(&this->items[index], &this->items[index + 1], sizeof(FastStringItem<T>) * (this->cnt - (UOSInt)index));
			}
			return oldVal;
		}
		return 0;
	}

	template <class T> T FastStringMap<T>::RemoveC(Text::CStringNN key)
	{
		UInt32 hash = this->crc.CalcDirect(key.v, key.leng);
		OSInt index = this->IndexOf(hash, key.v, key.leng);
		if (index >= 0)
		{
			T oldVal = this->items[index].val;
			this->items[index].s->Release();
			this->cnt--;
			if ((UOSInt)index < this->cnt)
			{
				MemCopyO(&this->items[index], &this->items[index + 1], sizeof(FastStringItem<T>) * (this->cnt - (UOSInt)index));
			}
			return oldVal;
		}
		return 0;
	}

	template <class T> T FastStringMap<T>::RemoveAt(UOSInt index)
	{
		if (index < this->cnt)
		{
			T oldVal = this->items[index].val;
			this->items[index].s->Release();
			this->cnt--;
			if ((UOSInt)index < this->cnt)
			{
				MemCopyO(&this->items[index], &this->items[index + 1], sizeof(FastStringItem<T>) * (this->cnt - (UOSInt)index));
			}
			return oldVal;
		}
		return 0;
	}

	template <class T> Bool FastStringMap<T>::IsEmpty() const
	{
		return this->cnt == 0;
	}

	template <class T> void FastStringMap<T>::Clear()
	{
		UOSInt i = this->cnt;
		while (i-- > 0)
		{
			this->items[i].s->Release();
		}
		this->cnt = 0;
	}

	template <class T> FastStringKeyIterator<T> FastStringMap<T>::KeyIterator() const
	{
		return FastStringKeyIterator<T>(this->items, this->cnt);
	}

	template <class T> UInt32 FastStringMap<T>::CalcHash(UnsafeArray<const UTF8Char> s, UOSInt len) const
	{
		return this->crc.CalcDirect(s, len);
	}
}

#endif
