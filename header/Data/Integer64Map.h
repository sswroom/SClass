#ifndef _SM_DATA_INTEGER64MAP
#define _SM_DATA_INTEGER64MAP
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListInt64.h"
#include "Data/ArrayMap.h"

namespace Data
{
	template <class T> class Integer64Map : public ArrayMap<Int64, T>
	{
	public:
		Integer64Map();
		virtual ~Integer64Map();
	};

	template <class T> Integer64Map<T>::Integer64Map() : ArrayMap<Int64, T>()
	{
		NEW_CLASS(this->keys, Data::ArrayListInt64());
	}

	template <class T> Integer64Map<T>::~Integer64Map()
	{
		DEL_CLASS(this->keys);
	}
};

#endif
