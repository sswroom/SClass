#ifndef _SM_DATA_NAMEDCLASS
#define _SM_DATA_NAMEDCLASS
#include "Data/Class.h"

namespace Data
{
	template <class T> class NamedClass : public Class
	{
	public:
		NamedClass(const T *refObj);
		virtual ~NamedClass();

		NN<T> CreateObject();
	};
}

template <class T> Data::NamedClass<T>::NamedClass(const T *refObj) : Class(refObj)
{
}

template <class T> Data::NamedClass<T>::~NamedClass()
{
}

template <class T> NN<T> Data::NamedClass<T>::CreateObject()
{
	NN<T> v;
	NEW_CLASSNN(v, T());
	return v;
}
#endif
