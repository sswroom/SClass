#ifndef _SM_TEXT_STRING
#define _SM_TEXT_STRING
#include "Data/IComparable.h"

namespace Text
{
	class String : public Data::IComparable
	{
	public:
		UOSInt leng;
		const UTF8Char *strVal;

		String(const UTF8Char *str);
		String(const UTF8Char *str, UTF8Char *strEnd);
		String(UOSInt leng);
		virtual ~String();

		virtual OSInt CompareTo(Data::IComparable *obj);
	};
}
#endif
