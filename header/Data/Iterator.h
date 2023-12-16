#ifndef _SM_DATA_ITERATOR
#define _SM_DATA_ITERATOR

namespace Data
{
	template <class T> class Iterator
	{
	public:
		virtual Bool HasNext() const = 0;
		virtual T Next() = 0;
	};
}
#endif
