#ifndef _SM_TEXT_CSTRING
#define _SM_TEXT_CSTRING
#include "Text/StringBase.h"

#define CSTR(str) Text::CString(UTF8STRC(str))
#define CSTRP(str, strEnd) {str, (UOSInt)(strEnd - str)}
#define CSTR_NULL {0, 0}
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

	FORCEINLINE UTF8Char *StrCSVJoinC(UTF8Char *oriStr, Text::CString *strs, UOSInt nStrs)
	{
		UOSInt i = 0;
		const UTF8Char *sptr;
		UTF8Char c;
		while (i < nStrs)
		{
			if (i)
				*oriStr++ = ',';
			*oriStr++ = '"';
			if ((sptr = strs[i].v) != 0)
			{
				while ((c = *sptr++) != 0)
				{
					if (c == '"')
					{
						*oriStr++ = '"';
					}
					*oriStr++ = c;
				}
			}
			*oriStr++ = '"';
			i++;
		}
		*oriStr = 0;
		return oriStr;
	}
}
#endif
