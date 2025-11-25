#ifndef _SM_DATA_ARRAYLISTSTRUTF8
#define _SM_DATA_ARRAYLISTSTRUTF8
#include "Data/SortableArrayList.hpp"

namespace Data
{
	class ArrayListStrUTF8 : public Data::SortableArrayList<UnsafeArrayOpt<const UTF8Char>>
	{
	public:
		ArrayListStrUTF8();
		ArrayListStrUTF8(UOSInt capacity);

		virtual NN<Data::ArrayList<UnsafeArrayOpt<const UTF8Char>>> Clone() const;
		virtual OSInt Compare(UnsafeArrayOpt<const UTF8Char> obj1, UnsafeArrayOpt<const UTF8Char> obj2) const;
		UnsafeArray<const UTF8Char> JoinNewStr() const;
		void DeleteAll();
	};
}
#endif
