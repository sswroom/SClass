#ifndef _SM_DATA_FASTSTRINGMAP
#define _SM_DATA_FASTSTRINGMAP
#include "MyMemory.h"
#include "Crypto/Hash/CRC32RC.h"
#include "Data/IMap.h"
#include "Data/ReadingList.h"
#include "Text/MyString.h"
#include "Text/String.h"

namespace Data
{
	template <class T> class FastStringMap : public IMap<Text::String*, T>, public ReadingList<T>
	{
	private:
		struct StringItem
		{
			UInt32 hash;
			Text::String *s;
			T val;
		};

		UOSInt capacity;
		UOSInt cnt;
		StringItem *items;
		Crypto::Hash::CRC32RC *crc;

		void Insert(UOSInt index, UInt32 hash, Text::String *s, T val);
	public:
		FastStringMap();
		FastStringMap(FastStringMap<T> *map);
		virtual ~FastStringMap();

		virtual UOSInt GetCount();
		virtual T GetItem(UOSInt index);
		Text::String *GetKey(UOSInt index);
		virtual OSInt IndexOf(UInt32 hash, const UTF8Char *s);
		OSInt IndexOf(Text::String *s);

		virtual T Put(Text::String *key, T val);
		T Put(const UTF8Char *key, T val);
		virtual T Get(Text::String *key);
		T Get(const UTF8Char *key);
		virtual T Remove(Text::String *key);
		T Remove(const UTF8Char *key);
		virtual Bool IsEmpty();
		virtual void Clear();
	};

	template <class T> void FastStringMap<T>::Insert(UOSInt index, UInt32 hash, Text::String *s, T val)
	{
		if (index > this->cnt)
		{
			index = this->cnt;
		}
		if (this->cnt == this->capacity)
		{
			this->capacity = this->capacity << 1;
			StringItem *newItems = MemAlloc(StringItem, this->capacity);
			if (index > 0)
			{
				MemCopyNO(newItems, this->items, sizeof(StringItem) * index);
			}
			newItems[index].hash = hash;
			newItems[index].s = s;
			newItems[index].val = val;
			if (index < this->cnt)
			{
				MemCopyNO(&newItems[index + 1], &this->items[index], sizeof(StringItem) * (this->cnt - index));
			}
			MemFree(this->items);
			this->items = newItems;
			this->cnt++;
		}
		else
		{
			if (index < this->cnt)
			{
				MemCopyO(&this->items[index + 1], &this->items[index], sizeof(StringItem) * (this->cnt - index));
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
		NEW_CLASS(this->crc, Crypto::Hash::CRC32RC());
		this->items = MemAlloc(StringItem, this->capacity);
	}

	template <class T> FastStringMap<T>::FastStringMap(FastStringMap<T> *map)
	{
		this->capacity = map->capacity;
		this->cnt = map->cnt;
		NEW_CLASS(this->crc, Crypto::Hash::CRC32RC());
		this->items = MemAlloc(StringItem, this->capacity);
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
		MemFree(this->items);
		DEL_CLASS(this->crc);
	}

	template <class T> UOSInt FastStringMap<T>::GetCount()
	{
		return this->cnt;
	}

	template <class T> T FastStringMap<T>::GetItem(UOSInt index)
	{
		if (index >= this->cnt)
		{
			return 0;
		}
		return this->items[index].val;
	}

	template <class T> Text::String *FastStringMap<T>::GetKey(UOSInt index)
	{
		if (index >= this->cnt)
		{
			return 0;
		}
		return this->items[index].s;
	}

	template <class T> OSInt FastStringMap<T>::IndexOf(UInt32 hash, const UTF8Char *s)
	{
		OSInt i;
		OSInt j;
		OSInt k;
		OSInt l;
		i = 0;
		j = (OSInt)this->cnt - 1;
		while (i <= j)
		{
			k = (i + j) >> 1;
			if (this->items[k].hash == hash)
			{
				l = this->items[k].s->CompareTo(s);
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
				}
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

	template <class T> OSInt FastStringMap<T>::IndexOf(Text::String *s)
	{
		UInt32 hash = this->crc->CalcDirect(s->v, s->leng);
		return IndexOf(hash, s->v);
	}

	template <class T> T FastStringMap<T>::Put(Text::String *key, T val)
	{
		UInt32 hash = this->crc->CalcDirect(key->v, key->leng);
		OSInt index = this->IndexOf(hash, key->v);
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

	template <class T> T FastStringMap<T>::Put(const UTF8Char *key, T val)
	{
		UOSInt len = Text::StrCharCnt(key);
		UInt32 hash = this->crc->CalcDirect(key, len);
		OSInt index = this->IndexOf(hash, key);
		if (index < 0)
		{
			this->Insert((UOSInt)~index, hash, Text::String::New(key, len), val);
			return 0;
		}
		else
		{
			T oldVal = this->items[index].val;
			this->items[index].val = val;
			return oldVal;
		}
	}

	template <class T> T FastStringMap<T>::Get(Text::String *key)
	{
		UInt32 hash = this->crc->CalcDirect(key->v, key->leng);
		OSInt index = this->IndexOf(hash, key->v);
		if (index >= 0)
		{
			return this->items[index].val;
		}
		return 0;
	}

	template <class T> T FastStringMap<T>::Get(const UTF8Char *key)
	{
		UOSInt len = Text::StrCharCnt(key);
		UInt32 hash = this->crc->CalcDirect(key, len);
		OSInt index = this->IndexOf(hash, key);
		if (index >= 0)
		{
			return this->items[index].val;
		}
		return 0;
	}

	template <class T> T FastStringMap<T>::Remove(Text::String *key)
	{
		UInt32 hash = this->crc->CalcDirect(key->v, key->leng);
		OSInt index = this->IndexOf(hash, key->v);
		if (index >= 0)
		{
			T oldVal = this->items[index].val;
			this->items[index].s->Release();
			this->cnt--;
			if ((UOSInt)index < this->cnt)
			{
				MemCopyO(&this->items[index], &this->items[index + 1], sizeof(StringItem) * (this->cnt - (UOSInt)index));
			}
			return oldVal;
		}
		return 0;
	}

	template <class T> T FastStringMap<T>::Remove(const UTF8Char *key)
	{
		UOSInt len = Text::StrCharCnt(key);
		UInt32 hash = this->crc->CalcDirect(key, len);
		OSInt index = this->IndexOf(hash, key);
		if (index >= 0)
		{
			T oldVal = this->items[index].val;
			this->items[index].s->Release();
			this->cnt--;
			if ((UOSInt)index < this->cnt)
			{
				MemCopyO(&this->items[index], &this->items[index + 1], sizeof(StringItem) * (this->cnt - (UOSInt)index));
			}
			return oldVal;
		}
		return 0;
	}

	template <class T> Bool FastStringMap<T>::IsEmpty()
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
}

#endif
