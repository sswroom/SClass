#ifndef _SM_DATA_TWINITEMNATIVE
#define _SM_DATA_TWINITEMNATIVE

namespace Data
{
	template <class T, class V> struct TwinItemNative
	{
	public:
		T key;
		V value;
	
		TwinItemNative(T key, V value)
		{
			this->key = key;
			this->value = value;
		}

		TwinItemNative(T key)
		{
			this->key = key;
			this->value = 0;
		}

		Bool operator==(TwinItemNative<T,V> item)
		{
			return this->key == item.key;
		}
	};
}
#endif
