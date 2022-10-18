#ifndef _SM_DATA_DATAMAP
#define _SM_DATA_DATAMAP

namespace Data
{
	template <class T, class V> class DataMap
	{
	public:
		DataMap();
		virtual ~DataMap();

		virtual V Put(T key, V val) = 0;
		virtual V Get(T key) const = 0;
		virtual V Remove(T key) = 0;
		virtual Bool IsEmpty() const = 0;
		virtual void Clear() = 0;
	};


	template <class T, class V> DataMap<T, V>::DataMap()
	{
	}

	template <class T, class V> DataMap<T, V>::~DataMap()
	{
	}
}

#endif
