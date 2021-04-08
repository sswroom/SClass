#ifndef _SM_DATA_INT32MAP
#define _SM_DATA_INT32MAP
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListInt32.h"
#include "Data/ArrayMap.h"

namespace Data
{
	template <class T> class Int32Map : public ArrayMap<Int32, T>
	{
	public:
		Int32Map();
		virtual ~Int32Map();
	};

	template <class T> Int32Map<T>::Int32Map() : ArrayMap<Int32, T>()
	{
		NEW_CLASS(this->keys, Data::ArrayListInt32());
	}

	template <class T> Int32Map<T>::~Int32Map()
	{
		DEL_CLASS(this->keys);
	}
}

#endif
