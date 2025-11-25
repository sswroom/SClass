#ifndef _SM_DATA_ICASESTRINGMAPNN
#define _SM_DATA_ICASESTRINGMAPNN
#include "Data/ArrayListICaseString.h"
#include "Data/StringMapNN.hpp"

namespace Data
{
	template <class T> class ICaseStringMapNN : public StringMapNN<T>
	{
	public:
		ICaseStringMapNN();
		ICaseStringMapNN(NN<const ICaseStringMapNN<T>> map);
		virtual ~ICaseStringMapNN();

		virtual NN<StringMapNN<T>> Clone() const;
	};


	template <class T> ICaseStringMapNN<T>::ICaseStringMapNN() : StringMapNN<T>()
	{
		this->keys.Delete();
		NEW_CLASSNN(this->keys, Data::ArrayListICaseString());
	}

	template <class T> ICaseStringMapNN<T>::ICaseStringMapNN(NN<const ICaseStringMapNN<T>> map) : StringMapNN<T>()
	{
		this->keys.Delete();
		NEW_CLASSNN(this->keys, Data::ArrayListICaseString());
		UOSInt i = 0;
		UOSInt j = map->keys->GetCount();
		while (i < j)
		{
			this->keys->Add(map->keys->GetItem(i)->Clone().Ptr());
			this->vals.Add(map->vals.GetItemNoCheck(i));
			i++;
		}
	}

	template <class T> ICaseStringMapNN<T>::~ICaseStringMapNN()
	{
	}

	template <class T> NN<StringMapNN<T>> ICaseStringMapNN<T>::Clone() const
	{
		NN<ICaseStringMapNN<T>> ret;
		NEW_CLASSNN(ret, ICaseStringMapNN<T>(NNTHIS));
		return ret;
	}
}

#endif
