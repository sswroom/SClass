#ifndef _SM_DATA_READINGLISTNN
#define _SM_DATA_READINGLISTNN
#include "Data/ReadingList.hpp"
namespace Data
{
	template <class T> class ReadingListNN : public Data::ReadingList<Optional<T>>
	{
	public:
		virtual UOSInt GetCount() const = 0;
		virtual Optional<T> GetItem(UOSInt index) const = 0;
		virtual NN<T> GetItemNoCheck(UOSInt index) const = 0;
	};
}
#endif
