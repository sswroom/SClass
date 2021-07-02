#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include <stdlib.h>

extern "C" OSInt MyString_StrCompare(const Char *str1, const Char *str2)
{
	OSInt i = 0;
	OSInt j = 0;
	while (1)
	{
		if (*str1 == 0 && *str2 == 0)
			return 0;
		if (*str1 == 0)
			return -1;
		if (*str2 == 0)
			return 1;

		if (*str1 >= 0x30 && *str1 <= 0x39 && *str2 >= 0x30 && *str2 <= 0x39)
		{
			i = 0;
			j = 0;
			while (*str1 >= 0x30 && *str1 <= 0x39)
			{
				i = i * 10 + ((*str1++) - 0x30);
			}
			while (*str2 >= 0x30 && *str2 <= 0x39)
			{
				j = j * 10 + ((*str2++) - 0x30);
			}

			if (i > j)
				return 1;
			else if (i < j)
				return -1;
		}
		else if (*str1 > *str2)
		{
			return 1;
		}
		else if (*str1 < *str2)
		{
			return -1;
		}
		else
		{
			str1++;
			str2++;
		}
	}
	return 0;
}

extern "C" OSInt MyString_StrCompareICase(const Char *str1, const Char *str2)
{
	OSInt i = 0;
	OSInt j = 0;
	Char c1;
	Char c2;
	while (1)
	{
		c1 = *str1;
		c2 = *str2;
		if (c1 == 0 && c2 == 0)
			return 0;
		if (c1 == 0)
			return -1;
		if (c2 == 0)
			return 1;

		if (c1 >= 0x30 && c1 <= 0x39 && c2 >= 0x30 && c2 <= 0x39)
		{
			i = 0;
			j = 0;
			while (*str1 >= 0x30 && *str1 <= 0x39)
			{
				i = i * 10 + ((*str1++) - 0x30);
			}
			while (*str2 >= 0x30 && *str2 <= 0x39)
			{
				j = j * 10 + ((*str2++) - 0x30);
			}

			if (i > j)
				return 1;
			else if (i < j)
				return -1;
		}
		else
		{
			if (c1 >= 'a' && c1 <= 'z')
				c1 -= 32;
			if (c2 >= 'a' && c2 <= 'z')
				c2 -= 32;

			if (c1 == c2)
			{
				str1++;
				str2++;
			}
			else if (c1 > c2)
			{
				return 1;
			}
			else
			{
				return -1;
			}
		}
	}
	return 0;
}

extern "C" OSInt MyString_StrCompareUTF16(const UTF16Char *str1, const UTF16Char *str2)
{
	OSInt i = 0;
	OSInt j = 0;
	while (1)
	{
		if (*str1 == 0 && *str2 == 0)
			return 0;
		if (*str1 == 0)
			return -1;
		if (*str2 == 0)
			return 1;

		if (*str1 >= 0x30 && *str1 <= 0x39 && *str2 >= 0x30 && *str2 <= 0x39)
		{
			i = 0;
			j = 0;
			while (*str1 >= 0x30 && *str1 <= 0x39)
			{
				i = i * 10 + ((*str1++) - 0x30);
			}
			while (*str2 >= 0x30 && *str2 <= 0x39)
			{
				j = j * 10 + ((*str2++) - 0x30);
			}

			if (i > j)
				return 1;
			else if (i < j)
				return -1;
		}
		else if (*str1 > *str2)
		{
			return 1;
		}
		else if (*str1 < *str2)
		{
			return -1;
		}
		else
		{
			str1++;
			str2++;
		}
	}
	return 0;
}

extern "C" OSInt MyString_StrCompareICaseUTF16(const UTF16Char *str1, const UTF16Char *str2)
{
	OSInt i = 0;
	OSInt j = 0;
	UTF16Char c1;
	UTF16Char c2;
	while (1)
	{
		c1 = *str1;
		c2 = *str2;
		if (c1 == 0 && c2 == 0)
			return 0;
		if (c1 == 0)
			return -1;
		if (c2 == 0)
			return 1;

		if (c1 >= 0x30 && c1 <= 0x39 && c2 >= 0x30 && c2 <= 0x39)
		{
			i = 0;
			j = 0;
			while (*str1 >= 0x30 && *str1 <= 0x39)
			{
				i = i * 10 + ((*str1++) - 0x30);
			}
			while (*str2 >= 0x30 && *str2 <= 0x39)
			{
				j = j * 10 + ((*str2++) - 0x30);
			}

			if (i > j)
				return 1;
			else if (i < j)
				return -1;
		}
		else
		{
			if (c1 >= 'a' && c1 <= 'z')
				c1 -= 32;
			if (c2 >= 'a' && c2 <= 'z')
				c2 -= 32;

			if (c1 == c2)
			{
				str1++;
				str2++;
			}
			else if (c1 > c2)
			{
				return 1;
			}
			else
			{
				return -1;
			}
		}
	}
	return 0;
}

extern "C" OSInt MyString_StrCompareUTF32(const UTF32Char *str1, const UTF32Char *str2)
{
	OSInt i = 0;
	OSInt j = 0;
	while (1)
	{
		if (*str1 == 0 && *str2 == 0)
			return 0;
		if (*str1 == 0)
			return -1;
		if (*str2 == 0)
			return 1;

		if (*str1 >= 0x30 && *str1 <= 0x39 && *str2 >= 0x30 && *str2 <= 0x39)
		{
			i = 0;
			j = 0;
			while (*str1 >= 0x30 && *str1 <= 0x39)
			{
				i = i * 10 + ((*str1++) - 0x30);
			}
			while (*str2 >= 0x30 && *str2 <= 0x39)
			{
				j = j * 10 + ((*str2++) - 0x30);
			}

			if (i > j)
				return 1;
			else if (i < j)
				return -1;
		}
		else if (*str1 > *str2)
		{
			return 1;
		}
		else if (*str1 < *str2)
		{
			return -1;
		}
		else
		{
			str1++;
			str2++;
		}
	}
	return 0;
}

extern "C" OSInt MyString_StrCompareICaseUTF32(const UTF32Char *str1, const UTF32Char *str2)
{
	OSInt i = 0;
	OSInt j = 0;
	UTF32Char c1;
	UTF32Char c2;
	while (1)
	{
		c1 = *str1;
		c2 = *str2;
		if (c1 == 0 && c2 == 0)
			return 0;
		if (c1 == 0)
			return -1;
		if (c2 == 0)
			return 1;

		if (c1 >= 0x30 && c1 <= 0x39 && c2 >= 0x30 && c2 <= 0x39)
		{
			i = 0;
			j = 0;
			while (*str1 >= 0x30 && *str1 <= 0x39)
			{
				i = i * 10 + ((*str1++) - 0x30);
			}
			while (*str2 >= 0x30 && *str2 <= 0x39)
			{
				j = j * 10 + ((*str2++) - 0x30);
			}

			if (i > j)
				return 1;
			else if (i < j)
				return -1;
		}
		else
		{
			if (c1 >= 'a' && c1 <= 'z')
				c1 -= 32;
			if (c2 >= 'a' && c2 <= 'z')
				c2 -= 32;

			if (c1 == c2)
			{
				str1++;
				str2++;
			}
			else if (c1 > c2)
			{
				return 1;
			}
			else
			{
				return -1;
			}
		}
	}
	return 0;
}

extern "C" UOSInt MyString_StrCharCnt(const Char *s)
{
	const Char *src = s;
	while (*src++) ;
	return src - s - 1;
}

extern "C" UOSInt MyString_StrCharCntUTF16(const UTF16Char *s)
{
	const UTF16Char *src = s;
	while (*src++) ;
	return (src - s - 1);
}

extern "C" UOSInt MyString_StrCharCntUTF32(const UTF32Char *s)
{
	const UTF32Char *src = s;
	while (*src++) ;
	return (src - s - 1);
}

#ifdef HAS_INT64
extern "C" UTF16Char *MyString_StrHexVal64VUTF16(UTF16Char *oriStr, UInt64 val)
{
	UTF16Char *tmp = &oriStr[16];
	*tmp = 0;
	UOSInt i = 16;
	while (i-- > 0)
	{
		*--tmp = MyString_STRHEXARR[val & 0xf];
		val >>= 4;
		if (val == 0)
		{
			while ((*oriStr++ = *tmp++) != 0);
			return oriStr - 1;
		}
	}
	return &oriStr[16];
}

extern "C" UTF16Char *MyString_StrHexVal64UTF16(UTF16Char *oriStr, UInt64 val)
{
	UTF16Char *tmp = &oriStr[16];
	UOSInt i = 16;
	while (i-- > 0)
	{
		*--tmp = MyString_STRHEXARR[val & 0xf];
		val >>= 4;
	}
	oriStr[16] = 0;
	return &oriStr[16];
}
#endif

extern "C" UTF16Char *MyString_StrHexVal32VUTF16(UTF16Char *oriStr, UInt32 val)
{
	UTF16Char *tmp = &oriStr[8];
	UOSInt i = 8;
	*tmp = 0;
	while (i-- > 0)
	{
		*--tmp = MyString_STRHEXARR[val & 0xf];
		val >>= 4;
		if (val == 0)
		{
			while ((*oriStr++ = *tmp++) != 0);
			return oriStr - 1;
		}
	}
	return &oriStr[8];
}

extern "C" UTF16Char *MyString_StrHexVal32UTF16(UTF16Char *oriStr, UInt32 val)
{
	UTF16Char *tmp = &oriStr[8];
	UOSInt i = 8;
	while (i-- > 0)
	{
		*--tmp = MyString_STRHEXARR[val & 0xf];
		val >>= 4;
	}
	oriStr[8] = 0;
	return &oriStr[8];
}

extern "C" UTF16Char *MyString_StrHexVal24UTF16(UTF16Char *oriStr, UInt32 val)
{
	UTF16Char *tmp = &oriStr[6];
	UOSInt i = 6;
	while (i-- > 0)
	{
		*--tmp = MyString_STRHEXARR[val & 0xf];
		val >>= 4;
	}
	oriStr[6] = 0;
	return &oriStr[6];
}

#ifdef HAS_INT64
extern "C" UTF32Char *MyString_StrHexVal64VUTF32(UTF32Char *oriStr, UInt64 val)
{
	UTF32Char *tmp = &oriStr[16];
	*tmp = 0;
	UOSInt i = 16;
	while (i-- > 0)
	{
		*--tmp = MyString_STRHEXARR[val & 0xf];
		val >>= 4;
		if (val == 0)
		{
			while ((*oriStr++ = *tmp++) != 0);
			return oriStr - 1;
		}
	}
	return &oriStr[16];
}

extern "C" UTF32Char *MyString_StrHexVal64UTF32(UTF32Char *oriStr, UInt64 val)
{
	UTF32Char *tmp = &oriStr[16];
	UOSInt i = 16;
	while (i-- > 0)
	{
		*--tmp = MyString_STRHEXARR[val & 0xf];
		val >>= 4;
	}
	oriStr[16] = 0;
	return &oriStr[16];
}
#endif

extern "C" UTF32Char *MyString_StrHexVal32VUTF32(UTF32Char *oriStr, UInt32 val)
{
	UTF32Char *tmp = &oriStr[8];
	UOSInt i = 8;
	*tmp = 0;
	while (i-- > 0)
	{
		*--tmp = MyString_STRHEXARR[val & 0xf];
		val >>= 4;
		if (val == 0)
		{
			while ((*oriStr++ = *tmp++) != 0);
			return oriStr - 1;
		}
	}
	return &oriStr[8];
}

extern "C" UTF32Char *MyString_StrHexVal32UTF32(UTF32Char *oriStr, UInt32 val)
{
	UTF32Char *tmp = &oriStr[8];
	UOSInt i = 8;
	while (i-- > 0)
	{
		*--tmp = MyString_STRHEXARR[val & 0xf];
		val >>= 4;
	}
	oriStr[8] = 0;
	return &oriStr[8];
}

extern "C" UTF32Char *MyString_StrHexVal24UTF32(UTF32Char *oriStr, UInt32 val)
{
	UTF32Char *tmp = &oriStr[6];
	UOSInt i = 6;
	while (i-- > 0)
	{
		*--tmp = MyString_STRHEXARR[val & 0xf];
		val >>= 4;
	}
	oriStr[6] = 0;
	return &oriStr[6];
}

extern "C" Int32 MyString_StrToInt32UTF8(const UTF8Char *intStr)
{
	Bool sign;
	Int32 retVal = 0;
	if (*intStr == '-')
	{
		sign = true;
		intStr++;
	}
	else
	{
		sign = false;
	}
	while (*intStr)
	{
		if (*intStr < '0' || *intStr > '9')
			return 0;
		retVal = retVal * 10 + *intStr - 48;
		intStr++;
	}
	if (sign)
		return -retVal;
	else
		return retVal;
}

extern "C" Int32 MyString_StrToInt32UTF16(const UTF16Char *intStr)
{
	Bool sign;
	Int32 retVal = 0;
	if (*intStr == '-')
	{
		sign = true;
		intStr++;
	}
	else
	{
		sign = false;
	}
	while (*intStr)
	{
		if (*intStr < '0' || *intStr > '9')
			return 0;
		retVal = retVal * 10 + *intStr - 48;
		intStr++;
	}
	if (sign)
		return -retVal;
	else
		return retVal;
}

extern "C" Int32 MyString_StrToInt32UTF32(const UTF32Char *intStr)
{
	Bool sign;
	Int32 retVal = 0;
	if (*intStr == '-')
	{
		sign = true;
		intStr++;
	}
	else
	{
		sign = false;
	}
	while (*intStr)
	{
		if (*intStr < '0' || *intStr > '9')
			return 0;
		retVal = retVal * 10 + *intStr - 48;
		intStr++;
	}
	if (sign)
		return -retVal;
	else
		return retVal;
}

#ifdef HAS_INT64
extern "C" Int64 MyString_StrToInt64UTF8(const UTF8Char *intStr)
{
	Bool sign;
	Int64 retVal = 0;
	if (*intStr == '-')
	{
		sign = true;
		intStr++;
	}
	else
	{
		sign = false;
	}
	while (*intStr)
	{
		if (*intStr < '0' || *intStr > '9')
			return 0;
		retVal = retVal * 10 + *intStr - 48;
		intStr++;
	}
	if (sign)
		return -retVal;
	else
		return retVal;
}

extern "C" Int64 MyString_StrToInt64UTF16(const UTF16Char *intStr)
{
	Bool sign;
	Int64 retVal = 0;
	if (*intStr == '-')
	{
		sign = true;
		intStr++;
	}
	else
	{
		sign = false;
	}
	while (*intStr)
	{
		if (*intStr < '0' || *intStr > '9')
			return 0;
		retVal = retVal * 10 + *intStr - 48;
		intStr++;
	}
	if (sign)
		return -retVal;
	else
		return retVal;
}

extern "C" Int64 MyString_StrToInt64UTF32(const UTF32Char *intStr)
{
	Bool sign;
	Int64 retVal = 0;
	if (*intStr == '-')
	{
		sign = true;
		intStr++;
	}
	else
	{
		sign = false;
	}
	while (*intStr)
	{
		if (*intStr < '0' || *intStr > '9')
			return 0;
		retVal = retVal * 10 + *intStr - 48;
		intStr++;
	}
	if (sign)
		return -retVal;
	else
		return retVal;
}
#endif
