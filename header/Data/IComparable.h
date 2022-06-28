#ifndef _SM_DATA_ICOMPARABLE
#define _SM_DATA_ICOMPARABLE

namespace Data
{
	class IComparable
	{
	public:
		typedef OSInt (__stdcall *CompareFunc)(void *obj1, void *obj2);
	public:
		virtual OSInt CompareTo(Data::IComparable *obj) const = 0;
	};
}
#endif
