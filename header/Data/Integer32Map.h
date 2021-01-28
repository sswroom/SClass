#ifndef _SM_DATA_INTEGER32MAP
#define _SM_DATA_INTEGER32MAP
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListInt32.h"
#include "Data/ArrayMap.h"

namespace Data
{
	template <class T> class Integer32Map : public ArrayMap<Int32, T>
	{
	public:
		Integer32Map();
		virtual ~Integer32Map();
	};

	template <class T> Integer32Map<T>::Integer32Map() : ArrayMap<Int32, T>()
	{
		NEW_CLASS(this->keys, Data::ArrayListInt32());
	}

	template <class T> Integer32Map<T>::~Integer32Map()
	{
		DEL_CLASS(this->keys);
	}
}

#endif
