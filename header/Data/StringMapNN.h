#ifndef _SM_DATA_STRINGMAPNN
#define _SM_DATA_STRINGMAPNN
#include "MyMemory.h"
#include "Data/ArrayCmpMapNN.h"
#include "Data/ArrayListString.h"
#include "Text/CString.h"

namespace Data
{
	template <class T> class StringMapNN : public ArrayCmpMapNN<Text::String*, T>
	{
	public:
		StringMapNN();
		StringMapNN(NN<const StringMapNN<T>> map);
		virtual ~StringMapNN();

		virtual Optional<T> Put(Text::String *key, NN<T> val);
		Optional<T> PutNN(NN<Text::String> key, NN<T> val);
		Optional<T> Put(Text::CStringNN key, NN<T> val);
		virtual Optional<T> Get(Text::String *key) const;
		Optional<T> GetNN(NN<Text::String> key) const;
		Optional<T> Get(Text::CStringNN key) const;
		virtual Optional<T> Remove(Text::String *key);
		Optional<T> RemoveNN(NN<Text::String> key);
		Optional<T> Remove(Text::CStringNN key);
		virtual Text::String *GetKey(UOSInt index) const;
		virtual void Clear();
		virtual NN<StringMapNN<T>> Clone() const;
	};


	template <class T> StringMapNN<T>::StringMapNN() : ArrayCmpMapNN<Text::String*, T>()
	{
		NEW_CLASS(this->keys, Data::ArrayListString());
	}

	template <class T> StringMapNN<T>::StringMapNN(NN<const StringMapNN<T>> map) : ArrayCmpMapNN<Text::String*, T>()
	{
		NEW_CLASS(this->keys, Data::ArrayListString());
		UOSInt i = 0;
		UOSInt j = map->keys->GetCount();
		while (i < j)
		{
			this->keys->Add(map->keys->GetItem(i)->Clone().Ptr());
			this->vals.Add(map->vals.GetItemNoCheck(i));
			i++;
		}
	}

	template <class T> StringMapNN<T>::~StringMapNN()
	{
		UOSInt i = this->keys->GetCount();
		while (i-- > 0)
		{
			this->keys->GetItem(i)->Release();
		}
		DEL_CLASS(this->keys);
	}

	template <class T> Optional<T> StringMapNN<T>::Put(Text::String *key, NN<T> val)
	{
		OSInt i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			Optional<T> oldVal = this->vals.GetItem((UOSInt)i);
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

	template <class T> Optional<T> StringMapNN<T>::PutNN(NN<Text::String> key, NN<T> val)
	{
		OSInt i;
		i = this->keys->SortedIndexOf(key.Ptr());
		if (i >= 0)
		{
			Optional<T> oldVal = this->vals.GetItem((UOSInt)i);
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

	template <class T> Optional<T> StringMapNN<T>::Put(Text::CStringNN key, NN<T> val)
	{
		OSInt i;
		i = ((Data::ArrayListString*)this->keys)->SortedIndexOfPtr(key.v, key.leng);
		if (i >= 0)
		{
			Optional<T> oldVal = this->vals.GetItem((UOSInt)i);
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

	template <class T> Optional<T> StringMapNN<T>::Get(Text::String *key) const
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

	template <class T> Optional<T> StringMapNN<T>::GetNN(NN<Text::String> key) const
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

	template <class T> Optional<T> StringMapNN<T>::Get(Text::CStringNN key) const
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

	template <class T> Optional<T> StringMapNN<T>::Remove(Text::String *key)
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

	template <class T> Optional<T> StringMapNN<T>::RemoveNN(NN<Text::String> key)
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

	template <class T> Optional<T> StringMapNN<T>::Remove(Text::CStringNN key)
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

	template <class T> Text::String *StringMapNN<T>::GetKey(UOSInt index) const
	{
		return this->keys->GetItem(index);
	}

	template <class T> void StringMapNN<T>::Clear()
	{
		UOSInt i;
		i = this->keys->GetCount();
		while (i-- > 0)
		{
			this->keys->RemoveAt(i)->Release();
		}
		this->vals.Clear();
	}

	template <class T> NN<StringMapNN<T>> StringMapNN<T>::Clone() const
	{
		NN<StringMapNN<T>> ret;
		NEW_CLASSNN(ret, StringMapNN<T>(*this));
		return ret;
	}
}

#endif
