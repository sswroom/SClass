#ifndef _SM_DATA_NAMEDCLASS
#define _SM_DATA_NAMEDCLASS
#include "Data/Class.h"

namespace Data
{
	template <class T> class NamedClass : public Class
	{
	public:
		NamedClass(T *refObj);
		virtual ~NamedClass();

		T *CreateObject();
	};
}

template <class T> Data::NamedClass<T>::NamedClass(T *refObj) : Class(refObj)
{
}

template <class T> Data::NamedClass<T>::~NamedClass()
{
}

template <class T> T *Data::NamedClass<T>::CreateObject()
{
	return NEW_CLASS_D(T());
}
#endif
