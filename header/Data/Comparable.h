#ifndef _SM_DATA_COMPARABLE
#define _SM_DATA_COMPARABLE

namespace Data
{
	class Comparable
	{
	public:
		virtual ~Comparable() {};
		virtual OSInt CompareTo(NN<Data::Comparable> obj) const = 0;
	};
}
#endif
