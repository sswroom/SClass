#ifndef _SM_DATA_ARRAYLISTICASESTRUTF8
#define _SM_DATA_ARRAYLISTICASESTRUTF8
#include "Data/ArrayListStrUTF8.h"

namespace Data
{
	class ArrayListICaseStrUTF8 : public Data::ArrayListStrUTF8
	{
	public:
		ArrayListICaseStrUTF8();
		ArrayListICaseStrUTF8(UOSInt capacity);

		virtual NN<Data::ArrayList<UnsafeArrayOpt<const UTF8Char>>> Clone() const;
		virtual OSInt CompareItem(UnsafeArrayOpt<const UTF8Char> obj1, UnsafeArrayOpt<const UTF8Char> obj2) const;
	};
}
#endif
