#ifndef _SM_DATA_TWINITEM
#define _SM_DATA_TWINITEM

namespace Data
{
	template <class T, class V> struct TwinItem
	{
	public:
		T key;
		V value;
	
		TwinItem(T key, V value)
		{
			this->key = key;
			this->value = value;
		}

		TwinItem(T key)
		{
			this->key = key;
			this->value = 0;
		}

		Bool operator==(TwinItem<T,V> item)
		{
			return this->key == item.key;
		}
	};
}
#endif
