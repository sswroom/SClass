#ifndef _SM_DATA_STRINGMAP
#define _SM_DATA_STRINGMAP
#include "MyMemory.h"
#include "Data/ArrayListString.h"
#include "Data/ArrayMap.h"

namespace Data
{
	template <class T> class StringMap : public ArrayMap<Text::String*, T>
	{
	public:
		StringMap();
		virtual ~StringMap();

		virtual T Put(Text::String *key, T val);
		T Put(const UTF8Char *key, T val);
		virtual T Get(Text::String *key);
		T Get(const UTF8Char *key);
		virtual T Remove(Text::String *key);
		virtual Text::String *GetKey(UOSInt index);
		virtual void Clear();
	};


	template <class T> StringMap<T>::StringMap() : ArrayMap<Text::String*, T>()
	{
		NEW_CLASS(this->keys, Data::ArrayListString());
	}

	template <class T> StringMap<T>::~StringMap()
	{
		UOSInt i = this->keys->GetCount();
		while (i-- > 0)
		{
			this->keys->GetItem(i)->Release();
		}
		DEL_CLASS(this->keys);
	}

	template <class T> T StringMap<T>::Put(Text::String *key, T val)
	{
		OSInt i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			T oldVal = this->vals->GetItem((UOSInt)i);
            this->vals->SetItem((UOSInt)i, val);
			return oldVal;
		}
		else
		{
			this->keys->Insert((UOSInt)~i, key->Clone());
			this->vals->Insert((UOSInt)~i, val);
			return 0;
		}
	}

	template <class T> T StringMap<T>::Put(const UTF8Char *key, T val)
	{
		OSInt i;
		i = ((Data::ArrayListString*)this->keys)->SortedIndexOfPtr(key);
		if (i >= 0)
		{
			T oldVal = this->vals->GetItem((UOSInt)i);
            this->vals->SetItem((UOSInt)i, val);
			return oldVal;
		}
		else
		{
			this->keys->Insert((UOSInt)~i, Text::String::New(key));
			this->vals->Insert((UOSInt)~i, val);
			return 0;
		}
	}

	template <class T> T StringMap<T>::Get(Text::String *key)
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

	template <class T> T StringMap<T>::Get(const UTF8Char *key)
	{
		OSInt i;
		i = ((Data::ArrayListString*)this->keys)->SortedIndexOfPtr(key);
		if (i >= 0)
		{
			return this->vals->GetItem((UOSInt)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> T StringMap<T>::Remove(Text::String *key)
	{
		OSInt i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			this->keys->RemoveAt((UOSInt)i)->Release();
			return this->vals->RemoveAt((UOSInt)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> Text::String *StringMap<T>::GetKey(UOSInt index)
	{
		return this->keys->GetItem(index);
	}

	template <class T> void StringMap<T>::Clear()
	{
		UOSInt i;
		i = this->keys->GetCount();
		while (i-- > 0)
		{
			this->keys->RemoveAt(i)->Release();
		}
		this->vals->Clear();
	}
}

#endif
