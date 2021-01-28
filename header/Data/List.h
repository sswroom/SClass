#ifndef _SM_DATA_LIST
#define _SM_DATA_LIST
namespace Data
{
	template <class T> class List
	{
	public:
		virtual UOSInt GetCount() = 0;
		virtual T GetItem(UOSInt index) = 0;
	};
}
#endif
