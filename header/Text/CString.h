#ifndef _SM_TEXT_CSTRING
#define _SM_TEXT_CSTRING
#include "Text/StringBase.h"
//#define DEBUGNULL
#if defined(DEBUGNULL)
#include <stdio.h>
#endif

#define CSTR(str) Text::CString(UTF8STRC(str))
#define CSTR_NULL Text::CString(0, 0)
#if defined(DEBUGNULL)
#define CSTRP(str, strEnd) Text::CString::FromPtrD(str, strEnd, __FILE__, __LINE__)
#else
#define CSTRP(str, strEnd) Text::CString(str, (UOSInt)(strEnd - str)
#endif

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

#if defined(DEBUGNULL)
		static CString FromPtrD(const UTF8Char *str, const UTF8Char *strEnd, const Char *fileName, UInt32 lineNum)
		{
			if (strEnd == 0)
			{
				printf("CSTRP found null at %s (%d)\r\n", fileName, lineNum);
				return CSTR_NULL;
			}
			return CString(str, (UOSInt)(strEnd - str));
		}
#endif

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
