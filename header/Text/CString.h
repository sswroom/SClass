#ifndef _SM_TEXT_CSTRING
#define _SM_TEXT_CSTRING
#include "UnsafeArrayOpt.h"
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
	struct CString
	{
		UnsafeArrayOpt<const UTF8Char> v;
		UOSInt leng;

		CString() = default;

		CString(std::nullptr_t)
		{
			this->v = UnsafeArray<const UTF8Char>::FromPtrNoCheck(0);
			this->leng = 0;
		}

		CString(CStringNN);
		CString(UnsafeArrayOpt<const UTF8Char> v, UOSInt leng)
		{
			this->v = v.Ptr();
			this->leng = leng;
		}

		static CString FromPtr(const UTF8Char *v)
		{
			UnsafeArray<const UTF8Char> nnv;
			if (!nnv.Set(v))
			{
				return CString(0, 0);
			}
			return CString(nnv, Text::StrCharCnt(nnv));
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

/*		CString Substring(UOSInt index) const
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
		}*/

		Bool IsNull() const
		{
			return this->v.IsNull();
		}

		Bool NotNull() const
		{
			return this->v.NotNull();
		}

		Bool operator==(Text::CString cstr) const
		{
			return this->v == cstr.v && this->leng == cstr.leng;
		}

		FORCEINLINE Bool SetTo(CStringNN &cstr) const;
		FORCEINLINE CStringNN OrEmpty() const;
		FORCEINLINE CStringNN Or(CStringNN cstr) const;
	};

	struct CStringNN : public StringBase<const UTF8Char>
	{
		CStringNN() = default;
		CStringNN(UnsafeArray<const UTF8Char> v, UOSInt leng)
		{
			this->v = v;
			this->leng = leng;
		}

		static CStringNN FromPtr(UnsafeArray<const UTF8Char> v)
		{
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

	FORCEINLINE CString::CString(Text::CStringNN cstr)
	{
		this->v = cstr.v;
		this->leng = cstr.leng;
	}

	FORCEINLINE Bool CString::SetTo(CStringNN &cstr) const
	{
		UnsafeArray<const UTF8Char> nnv;
		if (this->v.SetTo(nnv))
		{
			cstr.v = nnv;
			cstr.leng = this->leng;
			return true;			
		}
		return false;
	}

	FORCEINLINE CStringNN CString::OrEmpty() const
	{
		UnsafeArray<const UTF8Char> nnv;
		if (this->v.SetTo(nnv))
			return CStringNN(nnv, this->leng);
		else
			return CSTR("");
	}

	FORCEINLINE CStringNN CString::Or(CStringNN cstr) const
	{
		UnsafeArray<const UTF8Char> nnv;
		if (this->v.SetTo(nnv))
			return CStringNN(nnv, this->leng);
		else
			return cstr;
	}

	FORCEINLINE UnsafeArray<UTF8Char> StrCSVJoinC(UnsafeArray<UTF8Char> oriStr, Text::CString *strs, UOSInt nStrs)
	{
		UOSInt i = 0;
		Text::CStringNN cstr;
		UnsafeArray<const UTF8Char> sptr;
		UTF8Char c;
		while (i < nStrs)
		{
			if (i)
				*oriStr++ = ',';
			*oriStr++ = '"';
			if (strs[i].SetTo(cstr))
			{
				sptr = cstr.v;
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
