#ifndef _SM_TEXT_CSTRING
#define _SM_TEXT_CSTRING
#include "Text/StringBase.h"
//#define DEBUGNULL
#if defined(DEBUGNULL)
namespace
{
#include <stdio.h>
}
#endif

#define CSTR(str) Text::CStringNN(UTF8STRC(str))
#define CSTR_NULL Text::CString(0, 0)
#if defined(DEBUGNULL)
#define CSTRP(str, strEnd) Text::CString::FromPtrD(str, strEnd, __FILE__, __LINE__)
#else
#define CSTRP(str, strEnd) Text::CStringNN(str, (UOSInt)(strEnd - str))
#endif
#define CSTRPZ(str, strEnd) ((strEnd == 0)?CSTR(""):Text::CStringNN(str, (UOSInt)(strEnd - str)))

namespace Text
{
	struct CStringNN;
	struct CString : public StringBase<const UTF8Char>
	{
		CString() = default;

		CString(const Int32 *nul)
		{
			this->v = 0;
			this->leng = 0;
		}

		CString(UnsafeArray<const UTF8Char> v, UOSInt leng)
		{
			this->v = v.Ptr();
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


		CString Substring(UOSInt index) const
		{
			if (index >= this->leng)
			{
				return CString(this->v, 0);
			}
			else
			{
				return CString(this->v + index, this->leng - index);
			}
		}

		CString LTrim() const
		{
			UOSInt i = 0;
			while (i < this->leng)
			{
				if (this->v[i] != ' ' && this->v[i] != '\t')
					break;
				i++;
			}
			return CString(this->v + i, this->leng - i);
		}

		Bool IsNull() const
		{
			return this->v == 0;
		}

		FORCEINLINE CStringNN OrEmpty() const;
	};

	struct CStringNN : public CString
	{
		CStringNN() = default;
		CStringNN(UnsafeArray<const UTF8Char> v, UOSInt leng) : CString(v, leng)
		{
		}

		static CStringNN FromPtr(const UTF8Char *v)
		{
			if (v == 0)
			{
				return CSTR("");
			}
			return CStringNN(v, Text::StrCharCnt(v));
		}

		CStringNN Substring(UOSInt index) const
		{
			if (index >= this->leng)
			{
				return CStringNN(this->v, 0);
			}
			else
			{
				return CStringNN(this->v + index, this->leng - index);
			}
		}

		CStringNN LTrim() const
		{
			UOSInt i = 0;
			while (i < this->leng)
			{
				if (this->v[i] != ' ' && this->v[i] != '\t')
					break;
				i++;
			}
			return CStringNN(this->v + i, this->leng - i);
		}
	};

	FORCEINLINE CStringNN CString::OrEmpty() const
	{
		if (this->v)
			return CStringNN(this->v, this->leng);
		else
			return CSTR("");
	}

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
