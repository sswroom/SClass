#ifndef _SM_DATA_COMPARATOR
#define _SM_DATA_COMPARATOR

namespace Data
{
	template <class T> class Comparator
	{
	public:
		virtual ~Comparator() {};

		virtual OSInt Compare(T *a, T *b) = 0;
	};
}
#endif
