#ifndef _SM_TEXT_CSTRING
#define _SM_TEXT_CSTRING
#include "Text/StringBase.h"

#define CSTR(str) {UTF8STRC(str)}
#define CSTRP(str, strEnd) {str, (UOSInt)(strEnd - str)}
namespace Text
{
	struct CString : public StringBase<const UTF8Char>
	{
		CString()
		{
		}

		CString(const UTF8Char *v, UOSInt leng)
		{
			this->v = v;
			this->leng = leng;
		}

		static CString FromPtr(const UTF8Char *v)
		{
			if (v == 0)
			{
				return CString(0, 0);
			}
			return CString(v, Text::StrCharCnt(v));
		}

		CString Substring(UOSInt index)
		{
			if (index >= this->leng)
			{
				return {this->v, 0};
			}
			else
			{
				return {this->v + index, this->leng - index};
			}
		}
	};
}
#endif
