#ifndef _SM_DATA_ICASESTRINGMAPNN
#define _SM_DATA_ICASESTRINGMAPNN
#include "Data/ArrayListICaseString.h"
#include "Data/StringMapNN.h"

namespace Data
{
	template <class T> class ICaseStringMapNN : public StringMapNN<T>
	{
	public:
		ICaseStringMapNN();
		ICaseStringMapNN(NotNullPtr<const ICaseStringMapNN<T>> map);
		virtual ~ICaseStringMapNN();

		virtual NotNullPtr<StringMapNN<T>> Clone() const;
	};


	template <class T> ICaseStringMapNN<T>::ICaseStringMapNN() : StringMapNN<T>()
	{
		DEL_CLASS(this->keys);
		NEW_CLASS(this->keys, Data::ArrayListICaseString());
	}

	template <class T> ICaseStringMapNN<T>::ICaseStringMapNN(NotNullPtr<const ICaseStringMapNN<T>> map) : StringMapNN<T>()
	{
		DEL_CLASS(this->keys);
		NEW_CLASS(this->keys, Data::ArrayListICaseString());
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

	template <class T> NotNullPtr<StringMapNN<T>> ICaseStringMapNN<T>::Clone() const
	{
		NotNullPtr<ICaseStringMapNN<T>> ret;
		NEW_CLASSNN(ret, ICaseStringMapNN<T>(*this));
		return ret;
	}
}

#endif
