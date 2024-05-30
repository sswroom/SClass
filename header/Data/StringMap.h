#ifndef _SM_DATA_STRINGMAP
#define _SM_DATA_STRINGMAP
#include "MyMemory.h"
#include "Data/ArrayCmpMap.h"
#include "Data/ArrayListString.h"
#include "Text/CString.h"

namespace Data
{
	template <class T> class StringMap : public ArrayCmpMap<Text::String*, T>
	{
	public:
		StringMap();
		StringMap(const StringMap<T> *map);
		virtual ~StringMap();

		virtual T Put(Text::String *key, T val);
		T PutNN(NN<Text::String> key, T val);
		T Put(Text::CStringNN key, T val);
		virtual T Get(Text::String *key) const;
		T GetNN(NN<Text::String> key) const;
		T Get(Text::CStringNN key) const;
		virtual T Remove(Text::String *key);
		T RemoveNN(NN<Text::String> key);
		T Remove(Text::CStringNN key);
		virtual Text::String *GetKey(UOSInt index) const;
		virtual void Clear();
		virtual NN<StringMap<T>> Clone() const;
	};


	template <class T> StringMap<T>::StringMap() : ArrayCmpMap<Text::String*, T>()
	{
		NEW_CLASS(this->keys, Data::ArrayListString());
	}

	template <class T> StringMap<T>::StringMap(const StringMap<T> *map) : ArrayCmpMap<Text::String*, T>()
	{
		NEW_CLASS(this->keys, Data::ArrayListString());
		UOSInt i = 0;
		UOSInt j = map->keys->GetCount();
		while (i < j)
		{
			this->keys->Add(map->keys->GetItem(i)->Clone().Ptr());
			this->vals.Add(map->vals.GetItem(i));
			i++;
		}
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
			T oldVal = this->vals.GetItem((UOSInt)i);
            this->vals.SetItem((UOSInt)i, val);
			return oldVal;
		}
		else
		{
			this->keys->Insert((UOSInt)~i, key->Clone().Ptr());
			this->vals.Insert((UOSInt)~i, val);
			return 0;
		}
	}

	template <class T> T StringMap<T>::PutNN(NN<Text::String> key, T val)
	{
		OSInt i;
		i = this->keys->SortedIndexOf(key.Ptr());
		if (i >= 0)
		{
			T oldVal = this->vals.GetItem((UOSInt)i);
            this->vals.SetItem((UOSInt)i, val);
			return oldVal;
		}
		else
		{
			this->keys->Insert((UOSInt)~i, key->Clone().Ptr());
			this->vals.Insert((UOSInt)~i, val);
			return 0;
		}
	}

	template <class T> T StringMap<T>::Put(Text::CStringNN key, T val)
	{
		OSInt i;
		i = ((Data::ArrayListString*)this->keys)->SortedIndexOfPtr(key.v, key.leng);
		if (i >= 0)
		{
			T oldVal = this->vals.GetItem((UOSInt)i);
            this->vals.SetItem((UOSInt)i, val);
			return oldVal;
		}
		else
		{
			this->keys->Insert((UOSInt)~i, Text::String::New(key.v, key.leng).Ptr());
			this->vals.Insert((UOSInt)~i, val);
			return 0;
		}
	}

	template <class T> T StringMap<T>::Get(Text::String *key) const
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

	template <class T> T StringMap<T>::GetNN(NN<Text::String> key) const
	{
		OSInt i;
		i = this->keys->SortedIndexOf(key.Ptr());
		if (i >= 0)
		{
			return this->vals.GetItem((UOSInt)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> T StringMap<T>::Get(Text::CStringNN key) const
	{
		OSInt i;
		i = ((Data::ArrayListString*)this->keys)->SortedIndexOfPtr(key.v, key.leng);
		if (i >= 0)
		{
			return this->vals.GetItem((UOSInt)i);
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
			return this->vals.RemoveAt((UOSInt)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> T StringMap<T>::RemoveNN(NN<Text::String> key)
	{
		OSInt i;
		i = this->keys->SortedIndexOf(key.Ptr());
		if (i >= 0)
		{
			this->keys->RemoveAt((UOSInt)i)->Release();
			return this->vals.RemoveAt((UOSInt)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> T StringMap<T>::Remove(Text::CStringNN key)
	{
		OSInt i;
		i = ((Data::ArrayListString*)this->keys)->SortedIndexOfPtr(key.v, key.leng);
		if (i >= 0)
		{
			this->keys->RemoveAt((UOSInt)i)->Release();
			return this->vals.RemoveAt((UOSInt)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> Text::String *StringMap<T>::GetKey(UOSInt index) const
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
		this->vals.Clear();
	}

	template <class T> NN<StringMap<T>> StringMap<T>::Clone() const
	{
		NN<StringMap<T>> ret;
		NEW_CLASSNN(ret, StringMap<T>(this));
		return ret;
	}
}

#endif
