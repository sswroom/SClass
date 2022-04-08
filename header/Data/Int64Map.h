#ifndef _SM_DATA_INT64MAP
#define _SM_DATA_INT64MAP
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListInt64.h"
#include "Data/ArrayMap.h"

namespace Data
{
	template <class T> class Int64Map : public ArrayMap<Int64, T>
	{
	private:
		Data::ArrayListInt64 keyList;
	public:
		Int64Map();
		virtual ~Int64Map();
	};

	template <class T> Int64Map<T>::Int64Map() : ArrayMap<Int64, T>()
	{
		this->keys = &this->keyList;
	}

	template <class T> Int64Map<T>::~Int64Map()
	{
	}
}

#endif
