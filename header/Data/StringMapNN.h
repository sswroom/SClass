#ifndef _SM_DATA_STRINGMAPNN
#define _SM_DATA_STRINGMAPNN
#include "MyMemory.h"
#include "Data/ArrayCmpMapNN.h"
#include "Data/ArrayListString.h"
#include "Text/CString.h"

namespace Data
{
	template <class T> class StringMapNN : public ArrayCmpMapNN<Optional<Text::String>, T>
	{
	public:
		StringMapNN();
		StringMapNN(NN<const StringMapNN<T>> map);
		virtual ~StringMapNN();

		virtual Optional<T> Put(Optional<Text::String> key, NN<T> val);
		Optional<T> PutNN(NN<Text::String> key, NN<T> val);
		Optional<T> PutC(Text::CStringNN key, NN<T> val);
		virtual Optional<T> Get(Optional<Text::String> key) const;
		Optional<T> GetNN(NN<Text::String> key) const;
		Optional<T> GetC(Text::CStringNN key) const;
		virtual Optional<T> Remove(Optional<Text::String> key);
		Optional<T> RemoveNN(NN<Text::String> key);
		Optional<T> RemoveC(Text::CStringNN key);
		virtual Optional<Text::String> GetKey(UOSInt index) const;
		virtual void Clear();
		virtual NN<StringMapNN<T>> Clone() const;
		OSInt IndexOf(NN<Text::String> s) const;
		OSInt IndexOfC(Text::CStringNN s) const;
	};


	template <class T> StringMapNN<T>::StringMapNN() : ArrayCmpMapNN<Optional<Text::String>, T>()
	{
		NEW_CLASS(this->keys, Data::ArrayListString());
	}

	template <class T> StringMapNN<T>::StringMapNN(NN<const StringMapNN<T>> map) : ArrayCmpMapNN<Optional<Text::String>, T>()
	{
		NEW_CLASS(this->keys, Data::ArrayListString());
		UOSInt i = 0;
		UOSInt j = map->keys->GetCount();
		while (i < j)
		{
			this->keys->Add(Text::String::CopyOrNull(map->keys->GetItem(i)));
			this->vals.Add(map->vals.GetItemNoCheck(i));
			i++;
		}
	}

	template <class T> StringMapNN<T>::~StringMapNN()
	{
		NN<Text::String> s;
		UOSInt i = this->keys->GetCount();
		while (i-- > 0)
		{
			if (this->keys->GetItem(i).SetTo(s))
				s->Release();
		}
		DEL_CLASS(this->keys);
	}

	template <class T> Optional<T> StringMapNN<T>::Put(Optional<Text::String> key, NN<T> val)
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
			this->keys->Insert((UOSInt)~i, Text::String::CopyOrNull(key));
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
			this->keys->Insert((UOSInt)~i, key->Clone());
			this->vals.Insert((UOSInt)~i, val);
			return 0;
		}
	}

	template <class T> Optional<T> StringMapNN<T>::PutC(Text::CStringNN key, NN<T> val)
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
			this->keys->Insert((UOSInt)~i, Text::String::New(key.v, key.leng));
			this->vals.Insert((UOSInt)~i, val);
			return 0;
		}
	}

	template <class T> Optional<T> StringMapNN<T>::Get(Optional<Text::String> key) const
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

	template <class T> Optional<T> StringMapNN<T>::GetC(Text::CStringNN key) const
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

	template <class T> Optional<T> StringMapNN<T>::Remove(Optional<Text::String> key)
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

	template <class T> Optional<T> StringMapNN<T>::RemoveNN(NN<Text::String> key)
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

	template <class T> Optional<T> StringMapNN<T>::RemoveC(Text::CStringNN key)
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

	template <class T> Optional<Text::String> StringMapNN<T>::GetKey(UOSInt index) const
	{
		return this->keys->GetItem(index);
	}

	template <class T> void StringMapNN<T>::Clear()
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

	template <class T> NN<StringMapNN<T>> StringMapNN<T>::Clone() const
	{
		NN<StringMapNN<T>> ret;
		NEW_CLASSNN(ret, StringMapNN<T>(*this));
		return ret;
	}

	template <class T> OSInt StringMapNN<T>::IndexOf(NN<Text::String> s) const
	{
		return this->IndexOfC(s->ToCString());
	}

	template <class T> OSInt StringMapNN<T>::IndexOfC(Text::CStringNN s) const
	{
		OSInt i;
		OSInt j;
		OSInt k;
		OSInt l;
		i = 0;
		j = (OSInt)this->vals.GetCount() - 1;
		while (i <= j)
		{
			k = (i + j) >> 1;
			l = Text::String::OrEmpty(this->keys->GetItem((UOSInt)k))->CompareTo(s);
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
		return ~i;
	}
}

#endif
