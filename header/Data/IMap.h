#ifndef _SM_DATA_IMAP
#define _SM_DATA_IMAP

namespace Data
{
	template <class T, class V> class IMap
	{
	public:
		IMap();
		virtual ~IMap();

		virtual V Put(T key, V val) = 0;
		virtual V Get(T key) const = 0;
		virtual V Remove(T key) = 0;
		virtual Bool IsEmpty() const = 0;
		virtual void Clear() = 0;
	};


	template <class T, class V> IMap<T, V>::IMap()
	{
	}

	template <class T, class V> IMap<T, V>::~IMap()
	{
	}
}

#endif
