#ifndef _SM_DATA_READINGLIST
#define _SM_DATA_READINGLIST
namespace Data
{
	template <class T> class ReadingList
	{
	public:
		virtual UIntOS GetCount() const = 0;
		virtual T GetItem(UIntOS index) const = 0;
	};
}
#endif
