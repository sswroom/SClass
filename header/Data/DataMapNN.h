#ifndef _SM_DATA_DATAMAPNN
#define _SM_DATA_DATAMAPNN

namespace Data
{
	template <class T, class V> class DataMapNN
	{
	public:
		DataMapNN();
		virtual ~DataMapNN();

		virtual Optional<V> Put(T key, NN<V> val) = 0;
		virtual Optional<V> Get(T key) const = 0;
		virtual Optional<V> Remove(T key) = 0;
		virtual Bool IsEmpty() const = 0;
		virtual void Clear() = 0;
	};


	template <class T, class V> DataMapNN<T, V>::DataMapNN()
	{
	}

	template <class T, class V> DataMapNN<T, V>::~DataMapNN()
	{
	}
}

#endif
