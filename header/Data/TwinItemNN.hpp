#ifndef _SM_DATA_TWINITEMNN
#define _SM_DATA_TWINITEMNN

namespace Data
{
	template <class T, class V> struct TwinItemNN
	{
	public:
		T key;
		NN<V> value;
	
		TwinItemNN(T key, NN<V> value)
		{
			this->key = key;
			this->value = value;
		}

		TwinItemNN(T key)
		{
			this->key = key;
		}

		Bool operator==(TwinItemNN<T,V> item)
		{
			return this->key == item.key;
		}
	};
}
#endif
