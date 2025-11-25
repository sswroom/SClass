#ifndef _SM_DATA_STRINGMAP
#define _SM_DATA_STRINGMAP
#include "MyMemory.h"
#include "Data/ArrayCmpMap.hpp"
#include "Data/ArrayListString.h"
#include "Text/CString.h"

namespace Data
{
	template <class T> class StringMap : public ArrayCmpMap<Optional<Text::String>, T>
	{
	public:
		StringMap();
		StringMap(NN<const StringMap<T>> map);
		virtual ~StringMap();

		virtual T Put(Optional<Text::String> key, T val);
		T PutNN(NN<Text::String> key, T val);
		T Put(Text::CStringNN key, T val);
		virtual T Get(Optional<Text::String> key) const;
		T GetNN(NN<Text::String> key) const;
		T Get(Text::CStringNN key) const;
		virtual T Remove(Optional<Text::String> key);
		T RemoveNN(NN<Text::String> key);
		T Remove(Text::CStringNN key);
		virtual Optional<Text::String> GetKey(UOSInt index) const;
		virtual void Clear();
		virtual NN<StringMap<T>> Clone() const;
	};


	template <class T> StringMap<T>::StringMap() : ArrayCmpMap<Optional<Text::String>, T>(NEW_CLASS_D(Data::ArrayListString()))
	{
	}

	template <class T> StringMap<T>::StringMap(NN<const StringMap<T>> map) : ArrayCmpMap<Optional<Text::String>, T>(NEW_CLASS_D(Data::ArrayListString()))
	{
		UOSInt i = 0;
		UOSInt j = map->keys->GetCount();
		while (i < j)
		{
			this->keys->Add(Text::String::CopyOrNull(map->keys->GetItem(i)));
			this->vals.Add(map->vals.GetItem(i));
			i++;
		}
	}

	template <class T> StringMap<T>::~StringMap()
	{
		NN<Text::String> s;
		UOSInt i = this->keys->GetCount();
		while (i-- > 0)
		{
			if (this->keys->GetItem(i).SetTo(s))
				s->Release();
		}
		this->keys.Delete();
	}

	template <class T> T StringMap<T>::Put(Optional<Text::String> key, T val)
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
			this->keys->Insert((UOSInt)~i, Text::String::CopyOrNull(key));
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
			this->keys->Insert((UOSInt)~i, key->Clone());
			this->vals.Insert((UOSInt)~i, val);
			return 0;
		}
	}

	template <class T> T StringMap<T>::Put(Text::CStringNN key, T val)
	{
		OSInt i;
		i = NN<Data::ArrayListString>::ConvertFrom(this->keys)->SortedIndexOfPtr(key.v, key.leng);
		if (i >= 0)
		{
			T oldVal = this->vals.GetItem((UOSInt)i);
            this->vals.SetItem((UOSInt)i, val);
			return oldVal;
		}
		else
		{
			this->keys->Insert((UOSInt)~i, Text::String::New(key.v, key.leng));
			this->vals.Insert((UOSInt)~i, val);
			return 0;
		}
	}

	template <class T> T StringMap<T>::Get(Optional<Text::String> key) const
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
		i = NN<Data::ArrayListString>::ConvertFrom(this->keys)->SortedIndexOfPtr(key.v, key.leng);
		if (i >= 0)
		{
			return this->vals.GetItem((UOSInt)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> T StringMap<T>::Remove(Optional<Text::String> key)
	{
		NN<Text::String> s;
		OSInt i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			if (this->keys->RemoveAt((UOSInt)i).SetTo(s))
				s->Release();
			return this->vals.RemoveAt((UOSInt)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> T StringMap<T>::RemoveNN(NN<Text::String> key)
	{
		NN<Text::String> s;
		OSInt i;
		i = this->keys->SortedIndexOf(key.Ptr());
		if (i >= 0)
		{
			if (this->keys->RemoveAt((UOSInt)i).SetTo(s))
				s->Release();
			return this->vals.RemoveAt((UOSInt)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> T StringMap<T>::Remove(Text::CStringNN key)
	{
		NN<Text::String> s;
		OSInt i;
		i = ((Data::ArrayListString*)this->keys)->SortedIndexOfPtr(key.v, key.leng);
		if (i >= 0)
		{
			if (this->keys->RemoveAt((UOSInt)i).SetTo(s))
				s->Release();
			return this->vals.RemoveAt((UOSInt)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> Optional<Text::String> StringMap<T>::GetKey(UOSInt index) const
	{
		return this->keys->GetItem(index);
	}

	template <class T> void StringMap<T>::Clear()
	{
		NN<Text::String> s;
		UOSInt i;
		i = this->keys->GetCount();
		while (i-- > 0)
		{
			if (this->keys->RemoveAt(i).SetTo(s))
				s->Release();
		}
		this->vals.Clear();
	}

	template <class T> NN<StringMap<T>> StringMap<T>::Clone() const
	{
		NN<StringMap<T>> ret;
		NEW_CLASSNN(ret, StringMap<T>(NNTHIS));
		return ret;
	}
}

#endif
