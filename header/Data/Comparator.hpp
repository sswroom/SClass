#ifndef _SM_DATA_COMPARATOR
#define _SM_DATA_COMPARATOR

namespace Data
{
	template <class T> class Comparator
	{
	public:
		virtual ~Comparator() {};

		virtual IntOS Compare(T a, T b) const = 0;
	};
}
#endif
