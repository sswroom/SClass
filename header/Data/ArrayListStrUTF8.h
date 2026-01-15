#ifndef _SM_DATA_ARRAYLISTSTRUTF8
#define _SM_DATA_ARRAYLISTSTRUTF8
#include "Data/SortableArrayListObj.hpp"

namespace Data
{
	class ArrayListStrUTF8 : public Data::SortableArrayListObj<UnsafeArrayOpt<const UTF8Char>>
	{
	public:
		ArrayListStrUTF8();
		ArrayListStrUTF8(UIntOS capacity);

		virtual NN<Data::ArrayListObj<UnsafeArrayOpt<const UTF8Char>>> Clone() const;
		virtual IntOS Compare(UnsafeArrayOpt<const UTF8Char> obj1, UnsafeArrayOpt<const UTF8Char> obj2) const;
		UnsafeArray<const UTF8Char> JoinNewStr() const;
		void DeleteAll();
	};
}
#endif
