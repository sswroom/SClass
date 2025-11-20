#ifndef _SM_DATA_SYNCSTRINGMAP
#define _SM_DATA_SYNCSTRINGMAP
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListStrUTF8.h"
#include "Data/ArrayMap.hpp"

namespace Data
{
	template <class T> class SyncStringMap : public SyncArrayMap<const UTF8Char*, T>
	{
	public:
		SyncStringMap();
		virtual ~SyncStringMap();

		virtual T Put(const UTF8Char *key, T val);
		virtual T Get(const UTF8Char *key);
		virtual T Remove(const UTF8Char *key);
		virtual const UTF8Char *GetKey(OSInt index);
		virtual void Clear();
	};


	template <class T> SyncStringMap<T>::SyncStringMap() : SyncArrayMap<const UTF8Char*, T>()
	{
		NEW_CLASS(this->keys, Data::ArrayListStrUTF8());
	}

	template <class T> SyncStringMap<T>::~SyncStringMap()
	{
		OSInt i = this->keys->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew((UTF8Char*)this->keys->GetItem(i));
		}
		DEL_CLASS(this->keys);
	}

	template <class T> T SyncStringMap<T>::Put(const UTF8Char *key, T val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		OSInt i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			T oldVal = this->vals->GetItem(i);
            this->vals->SetItem(i, val);
			return oldVal;
		}
		else
		{
			this->keys->Insert(~i, Text::StrCopyNew(key));
			this->vals->Insert(~i, val);
			return 0;
		}
	}

	template <class T> T SyncStringMap<T>::Get(const UTF8Char *key)
	{
		Sync::MutexUsage mutUsage(this->mut);
		OSInt i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			return this->vals->GetItem(i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> T SyncStringMap<T>::Remove(const UTF8Char *key)
	{
		Sync::MutexUsage mutUsage(this->mut);
		OSInt i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			Text::StrDelNew(this->keys->RemoveAt(i));
			return this->vals->RemoveAt(i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> const UTF8Char *SyncStringMap<T>::GetKey(OSInt index)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->keys->GetItem(index);
	}

	template <class T> void SyncStringMap<T>::Clear()
	{
		Sync::MutexUsage mutUsage(this->mut);
		OSInt i;
		i = this->keys->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(this->keys->RemoveAt(i));
		}
		this->vals->Clear();
	}
}

#endif
