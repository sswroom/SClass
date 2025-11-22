#ifndef _SM_DATA_READINGLIST
#define _SM_DATA_READINGLIST
namespace Data
{
	template <class T> class ReadingList
	{
	public:
		virtual UOSInt GetCount() const = 0;
		virtual T GetItem(UOSInt index) const = 0;
	};
}
#endif
