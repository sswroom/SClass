#ifndef _SM_DATA_READINGLIST
#define _SM_DATA_READINGLIST
namespace Data
{
	template <class T> class ReadingList
	{
	public:
		virtual UOSInt GetCount() = 0;
		virtual T GetItem(UOSInt index) = 0;
	};
}
#endif
