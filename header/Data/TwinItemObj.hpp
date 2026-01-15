#ifndef _SM_DATA_TWINITEMOBJ
#define _SM_DATA_TWINITEMOBJ

namespace Data
{
	template <class T, class V> struct TwinItemObj
	{
	public:
		T key;
		V value;
	
		TwinItemObj(T key, V value)
		{
			this->key = key;
			this->value = value;
		}

		TwinItemObj(T key)
		{
			this->key = key;
			this->value = nullptr;
		}

		TwinItemObj(std::nullptr_t)
		{
			this->key = nullptr;
			this->value = nullptr;
		}

		Bool operator==(TwinItemObj<T,V> item)
		{
			return this->key == item.key;
		}
	};
}
#endif
