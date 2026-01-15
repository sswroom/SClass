#ifndef _SM_DATA_FASTSTRINGMAPOBJ
#define _SM_DATA_FASTSTRINGMAPOBJ
#include "MyMemory.h"
#include "Crypto/Hash/CRC32RC.h"
#include "Data/ListMap.hpp"
#include "Data/ReadingList.hpp"
#include "Text/MyString.h"
#include "Text/String.h"

namespace Data
{
	template <class T> struct FastStringObjItem
	{
		UInt32 hash;
		NN<Text::String> s;
		T val;
	};

	template <class T> class FastStringObjKeyIterator
	{
	private:
		UnsafeArray<const FastStringObjItem<T>> arr;
		UIntOS cnt;
	public:
		FastStringObjKeyIterator(UnsafeArray<const FastStringObjItem<T>> arr, UIntOS cnt)
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

	template <class T> class FastStringMapObj : public ListMap<Optional<Text::String>, T>
	{
	private:
		UIntOS capacity;
		UIntOS cnt;
		UnsafeArray<FastStringObjItem<T>> items;
		Crypto::Hash::CRC32RC crc;

		void Insert(UIntOS index, UInt32 hash, NN<Text::String> s, T val);
	public:
		FastStringMapObj();
		FastStringMapObj(NN<const FastStringMapObj<T>> map);
		virtual ~FastStringMapObj();

		virtual UIntOS GetCount() const;
		virtual T GetItem(UIntOS index) const;
		virtual Optional<Text::String> GetKey(UIntOS index) const;
		virtual IntOS IndexOf(UInt32 hash, UnsafeArray<const UTF8Char> s, UIntOS len) const;
		IntOS IndexOf(NN<Text::String> s) const;
		IntOS IndexOfC(Text::CStringNN s) const;

		virtual T Put(Optional<Text::String> key, T val);
		T PutNN(NN<Text::String> key, T val);
		T PutC(Text::CStringNN key, T val);
		virtual T Get(Optional<Text::String> key) const;
		T GetNN(NN<Text::String> key) const;
		T GetC(Text::CStringNN key) const;
		virtual T Remove(Optional<Text::String> key);
		T RemoveNN(NN<Text::String> key);
		T RemoveC(Text::CStringNN key);
		T RemoveAt(UIntOS index);
		virtual Bool IsEmpty() const;
		virtual void Clear();
		FastStringObjKeyIterator<T> KeyIterator() const;
		
		UInt32 CalcHash(UnsafeArray<const UTF8Char> s, UIntOS len) const;
	};

	template <class T> void FastStringMapObj<T>::Insert(UIntOS index, UInt32 hash, NN<Text::String> s, T val)
	{
		if (index > this->cnt)
		{
			index = this->cnt;
		}
		if (this->cnt == this->capacity)
		{
			this->capacity = this->capacity << 1;
			UnsafeArray<FastStringObjItem<T>> newItems = MemAllocArr(FastStringObjItem<T>, this->capacity);
			if (index > 0)
			{
				MemCopyNO(newItems.Ptr(), this->items.Ptr(), sizeof(FastStringObjItem<T>) * index);
			}
			newItems[index].hash = hash;
			newItems[index].s = s;
			newItems[index].val = val;
			if (index < this->cnt)
			{
				MemCopyNO(&newItems[index + 1], &this->items[index], sizeof(FastStringObjItem<T>) * (this->cnt - index));
			}
			MemFreeArr(this->items);
			this->items = newItems;
			this->cnt++;
		}
		else
		{
			if (index < this->cnt)
			{
				MemCopyO(&this->items[index + 1], &this->items[index], sizeof(FastStringObjItem<T>) * (this->cnt - index));
			}
			this->items[index].hash = hash;
			this->items[index].s = s;
			this->items[index].val = val;
			this->cnt++;
		}
	}

	template <class T> FastStringMapObj<T>::FastStringMapObj()
	{
		this->capacity = 64;
		this->cnt = 0;
		this->items = MemAllocArr(FastStringObjItem<T>, this->capacity);
	}

	template <class T> FastStringMapObj<T>::FastStringMapObj(NN<const FastStringMapObj<T>> map)
	{
		this->capacity = map->capacity;
		this->cnt = map->cnt;
		this->items = MemAllocArr(FastStringObjItem<T>, this->capacity);
		UIntOS i = 0;
		UIntOS j = this->cnt;
		while (i < j)
		{
			this->items[i].hash = map->items[i].hash;
			this->items[i].s = map->items[i].s->Clone();
			this->items[i].val = map->items[i].val;
			i++;
		}
	}

	template <class T> FastStringMapObj<T>::~FastStringMapObj()
	{
		UIntOS i = this->cnt;
		while (i-- > 0)
		{
			this->items[i].s->Release();
		}
		MemFreeArr(this->items);
	}

	template <class T> UIntOS FastStringMapObj<T>::GetCount() const
	{
		return this->cnt;
	}

	template <class T> T FastStringMapObj<T>::GetItem(UIntOS index) const
	{
		if (index >= this->cnt)
		{
			return 0;
		}
		return this->items[index].val;
	}

	template <class T> Optional<Text::String> FastStringMapObj<T>::GetKey(UIntOS index) const
	{
		if (index >= this->cnt)
		{
			return nullptr;
		}
		return this->items[index].s;
	}

	template <class T> IntOS FastStringMapObj<T>::IndexOf(UInt32 hash, UnsafeArray<const UTF8Char> s, UIntOS len) const
	{
		IntOS i;
		IntOS j;
		IntOS k;
//		IntOS l;
		i = 0;
		j = (IntOS)this->cnt - 1;
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

	template <class T> IntOS FastStringMapObj<T>::IndexOf(NN<Text::String> s) const
	{
		UInt32 hash = this->crc.CalcDirect(s->v, s->leng);
		return IndexOf(hash, s->v, s->leng);
	}

	template <class T> IntOS FastStringMapObj<T>::IndexOfC(Text::CStringNN s) const
	{
		UInt32 hash = this->crc.CalcDirect(s.v, s.leng);
		return IndexOf(hash, s.v, s.leng);
	}

	template <class T> T FastStringMapObj<T>::Put(Optional<Text::String> key, T val)
	{
		return PutNN(Text::String::OrEmpty(key), val);
	}

	template <class T> T FastStringMapObj<T>::PutNN(NN<Text::String> key, T val)
	{
		UInt32 hash = this->crc.CalcDirect(key->v, key->leng);
		IntOS index = this->IndexOf(hash, key->v, key->leng);
		if (index < 0)
		{
			this->Insert((UIntOS)~index, hash, key->Clone(), val);
			return 0;
		}
		else
		{
			T oldVal = this->items[index].val;
			this->items[index].val = val;
			return oldVal;
		}
	}

	template <class T> T FastStringMapObj<T>::PutC(Text::CStringNN key, T val)
	{
		UInt32 hash = this->crc.CalcDirect(key.v, key.leng);
		IntOS index = this->IndexOf(hash, key.v, key.leng);
		if (index < 0)
		{
			this->Insert((UIntOS)~index, hash, Text::String::New(key.v, key.leng), val);
			return 0;
		}
		else
		{
			T oldVal = this->items[index].val;
			this->items[index].val = val;
			return oldVal;
		}
	}

	template <class T> T FastStringMapObj<T>::Get(Optional<Text::String> key) const
	{
		return GetNN(Text::String::OrEmpty(key));
	}

	template <class T> T FastStringMapObj<T>::GetNN(NN<Text::String> key) const
	{
		UInt32 hash = this->crc.CalcDirect(key->v, key->leng);
		IntOS index = this->IndexOf(hash, key->v, key->leng);
		if (index >= 0)
		{
			return this->items[index].val;
		}
		return 0;
	}

	template <class T> T FastStringMapObj<T>::GetC(Text::CStringNN key) const
	{
		UInt32 hash = this->crc.CalcDirect(key.v, key.leng);
		IntOS index = this->IndexOf(hash, key.v, key.leng);
		if (index >= 0)
		{
			return this->items[index].val;
		}
		return 0;
	}

	template <class T> T FastStringMapObj<T>::Remove(Optional<Text::String> key)
	{
		return RemoveNN(Text::String::OrEmpty(key));
	}

	template <class T> T FastStringMapObj<T>::RemoveNN(NN<Text::String> key)
	{
		UInt32 hash = this->crc.CalcDirect(key->v, key->leng);
		IntOS index = this->IndexOf(hash, key->v, key->leng);
		if (index >= 0)
		{
			T oldVal = this->items[index].val;
			this->items[index].s->Release();
			this->cnt--;
			if ((UIntOS)index < this->cnt)
			{
				MemCopyO(&this->items[index], &this->items[index + 1], sizeof(FastStringObjItem<T>) * (this->cnt - (UIntOS)index));
			}
			return oldVal;
		}
		return 0;
	}

	template <class T> T FastStringMapObj<T>::RemoveC(Text::CStringNN key)
	{
		UInt32 hash = this->crc.CalcDirect(key.v, key.leng);
		IntOS index = this->IndexOf(hash, key.v, key.leng);
		if (index >= 0)
		{
			T oldVal = this->items[index].val;
			this->items[index].s->Release();
			this->cnt--;
			if ((UIntOS)index < this->cnt)
			{
				MemCopyO(&this->items[index], &this->items[index + 1], sizeof(FastStringObjItem<T>) * (this->cnt - (UIntOS)index));
			}
			return oldVal;
		}
		return 0;
	}

	template <class T> T FastStringMapObj<T>::RemoveAt(UIntOS index)
	{
		if (index < this->cnt)
		{
			T oldVal = this->items[index].val;
			this->items[index].s->Release();
			this->cnt--;
			if ((UIntOS)index < this->cnt)
			{
				MemCopyO(&this->items[index], &this->items[index + 1], sizeof(FastStringObjItem<T>) * (this->cnt - (UIntOS)index));
			}
			return oldVal;
		}
		return 0;
	}

	template <class T> Bool FastStringMapObj<T>::IsEmpty() const
	{
		return this->cnt == 0;
	}

	template <class T> void FastStringMapObj<T>::Clear()
	{
		UIntOS i = this->cnt;
		while (i-- > 0)
		{
			this->items[i].s->Release();
		}
		this->cnt = 0;
	}

	template <class T> FastStringObjKeyIterator<T> FastStringMapObj<T>::KeyIterator() const
	{
		return FastStringObjKeyIterator<T>(this->items, this->cnt);
	}

	template <class T> UInt32 FastStringMapObj<T>::CalcHash(UnsafeArray<const UTF8Char> s, UIntOS len) const
	{
		return this->crc.CalcDirect(s, len);
	}
}

#endif
