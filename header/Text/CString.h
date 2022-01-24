#ifndef _SM_TEXT_CSTRING
#define _SM_TEXT_CSTRING
#include "Text/MyString.h"
namespace Text
{
	struct CString
	{
		const UTF8Char *v;
		UOSInt leng;

		CString()
		{
		}
		
		CString(const UTF8Char *v, UOSInt leng)
		{
			this->v = v;
			this->leng = leng;
		}

		FORCEINLINE Bool Equals(const UTF8Char *s, UOSInt len)
		{
			return Text::StrEqualsC(v, this->leng, s, len);
		}

		FORCEINLINE Bool StartsWith(const UTF8Char *s, UOSInt len)
		{
			return Text::StrStartsWithC(v, this->leng, s, len);
		}
	};
}
#endif
