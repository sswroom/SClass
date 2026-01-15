#ifndef _SM_DATA_TWINITEMNATIVEOBJ
#define _SM_DATA_TWINITEMNATIVEOBJ

namespace Data
{
	template <class T, class V> struct TwinItemNativeObj
	{
	public:
		T key;
		V value;
	
		TwinItemNativeObj(T key, V value)
		{
			this->key = key;
			this->value = value;
		}

		TwinItemNativeObj(T key)
		{
			this->key = key;
			this->value = nullptr;
		}

		TwinItemNativeObj(std::nullptr_t)
		{
			this->key = 0;
			this->value = nullptr;
		}

		Bool operator==(TwinItemNativeObj<T,V> item)
		{
			return this->key == item.key;
		}
	};
}
#endif
