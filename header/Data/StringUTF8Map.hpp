#ifndef _SM_DATA_STRINGUTF8MAP
#define _SM_DATA_STRINGUTF8MAP
#include "MyMemory.h"
#include "Data/ArrayCmpMap.hpp"
#include "Data/ArrayListStrUTF8.h"
#include "Text/MyString.h"

namespace Data
{
	template <class T> class StringUTF8Map : public ArrayCmpMap<UnsafeArrayOpt<const UTF8Char>, T>
	{		
	public:
		StringUTF8Map();
		virtual ~StringUTF8Map();

		virtual T Put(UnsafeArrayOpt<const UTF8Char> key, T val);
		virtual T Get(UnsafeArrayOpt<const UTF8Char> key) const;
		virtual T Remove(UnsafeArrayOpt<const UTF8Char> key);
		virtual UnsafeArrayOpt<const UTF8Char> GetKey(UOSInt index) const;
		virtual void Clear();
	};


	template <class T> StringUTF8Map<T>::StringUTF8Map() : ArrayCmpMap<UnsafeArrayOpt<const UTF8Char>, T>(NEW_CLASS_D(Data::ArrayListStrUTF8()))
	{
	}

	template <class T> StringUTF8Map<T>::~StringUTF8Map()
	{
		UnsafeArray<const UTF8Char> nnkey;
		UOSInt i = this->keys->GetCount();
		while (i-- > 0)
		{
			if (this->keys->GetItem(i).SetTo(nnkey))
			{
				Text::StrDelNew(nnkey);
			}
		}
		this->keys.Delete();
	}

	template <class T> T StringUTF8Map<T>::Put(UnsafeArrayOpt<const UTF8Char> key, T val)
	{
		UnsafeArray<const UTF8Char> nnkey = key.Or(U8STR(""));
		OSInt i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			T oldVal = this->vals.GetItem((UOSInt)i);
            this->vals.SetItem((UOSInt)i, val);
			return oldVal;
		}
		else
		{
			this->keys->Insert((UOSInt)~i, Text::StrCopyNew(nnkey));
			this->vals.Insert((UOSInt)~i, val);
			return 0;
		}
	}

	template <class T> T StringUTF8Map<T>::Get(UnsafeArrayOpt<const UTF8Char> key) const
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

	template <class T> T StringUTF8Map<T>::Remove(UnsafeArrayOpt<const UTF8Char> key)
	{
		OSInt i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			UnsafeArray<const UTF8Char> nnkey;
			if (this->keys->RemoveAt((UOSInt)i).SetTo(nnkey))
			{
				Text::StrDelNew(nnkey);
			}
			return this->vals.RemoveAt((UOSInt)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> UnsafeArrayOpt<const UTF8Char> StringUTF8Map<T>::GetKey(UOSInt index) const
	{
		return this->keys->GetItem(index);
	}

	template <class T> void StringUTF8Map<T>::Clear()
	{
		UnsafeArray<const UTF8Char> nnkey;
		UOSInt i;
		i = this->keys->GetCount();
		while (i-- > 0)
		{
			if (this->keys->RemoveAt(i).SetTo(nnkey))
				Text::StrDelNew(nnkey);
		}
		this->vals.Clear();
	}
}

#endif
