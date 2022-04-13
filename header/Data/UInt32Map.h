#ifndef _SM_DATA_UINT32MAP
#define _SM_DATA_UINT32MAP
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListUInt32.h"
#include "Data/ArrayMap.h"

namespace Data
{
	template <class T> class UInt32Map : public ArrayMap<UInt32, T>
	{
	private:
		Data::ArrayListUInt32 keyList;
	public:
		UInt32Map();
		virtual ~UInt32Map();
	};

	template <class T> UInt32Map<T>::UInt32Map() : ArrayMap<UInt32, T>()
	{
		this->keys = &this->keyList;
	}

	template <class T> UInt32Map<T>::~UInt32Map()
	{
	}
}

#endif
