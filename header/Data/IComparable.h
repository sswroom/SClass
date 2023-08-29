#ifndef _SM_DATA_ICOMPARABLE
#define _SM_DATA_ICOMPARABLE

namespace Data
{
	class IComparable
	{
	public:
		virtual OSInt CompareTo(Data::IComparable *obj) const = 0;
	};
}
#endif
