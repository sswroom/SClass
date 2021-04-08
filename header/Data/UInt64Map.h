#ifndef _SM_DATA_UINT64MAP
#define _SM_DATA_UINT64MAP
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListUInt64.h"
#include "Data/ArrayMap.h"

namespace Data
{
	template <class T> class UInt64Map : public ArrayMap<UInt64, T>
	{
	public:
		UInt64Map();
		virtual ~UInt64Map();
	};

	template <class T> UInt64Map<T>::UInt64Map() : ArrayMap<UInt64, T>()
	{
		NEW_CLASS(this->keys, Data::ArrayListUInt64());
	}

	template <class T> UInt64Map<T>::~UInt64Map()
	{
		DEL_CLASS(this->keys);
	}
}

#endif
