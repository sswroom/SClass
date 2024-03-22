#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF.h"

UTF8Char *Text::StrDouble(UTF8Char *oriStr, Double val, UOSInt sigFig, NotNullPtr<const DoubleStyle> style)
{
	if (val == 0)
	{
		if (Math::IsNeg(val))
		{
			return Text::StrConcatC(oriStr, UTF8STRC("-0"));
		}
		else
		{
			return Text::StrConcatC(oriStr, UTF8STRC("0"));
		}
	}
	else if (Math::IsNAN(val))
	{
		return Text::StrConcatC(oriStr, style->nanStr, style->nanLen);
	}
	else if (val < 0)
	{
		*oriStr++ = '-';
		val = -val;
	}
	if (Math::IsInfinity(val))
	{
		return Text::StrConcatC(oriStr, style->infStr, style->infLen);
	}
	Double addBase = 5.0e-11;
	UOSInt i;
	if (sigFig < 10)
	{
		sigFig = 10;
	}
	else
	{
		i = 10;
		while (i < sigFig)
		{
			addBase *= 0.1;
			i++;
		}
	}
	i = sigFig >> 1;
	Int32 ex = -10000 + (Int32)(Math_Log10(val) + 10000);
	Int32 iVal;
	val = val * Math_Pow(10.0, -ex - 1) + addBase;
	if (ex >= (OSInt)(sigFig + 1) || ex <= -4)
	{
		val = val * 100.0;
		iVal = (Int32)val;
		val = val - iVal;
		oriStr[0] = MyString_StrDigit100U8[iVal * 2];
		oriStr[1] = '.';
		oriStr[2] = MyString_StrDigit100U8[iVal * 2 + 1];
		oriStr += 3;
		i--;

		if (val > 1.0e-10)
		{
			while (val > 1.0e-10)
			{
				val = val * 100.0;
				iVal = (Int32)val;
				val = val - iVal;
				WriteNInt16((UInt8*)&oriStr[0], ReadNInt16(&MyString_StrDigit100U8[iVal * 2]));
				oriStr += 2;
				if (--i <= 0)
					break;
			}
			if (oriStr[-1] == '0')
			{
				oriStr -= 1;
			}
		}
		oriStr[0] = 'e';
		if (ex < 0)
		{
			oriStr[1] = '-';
			ex = -ex;
		}
		else
		{
			oriStr[1] = '+';
		}
		UTF8Char *tmpStr = oriStr + 13;
		oriStr += 2;
		*tmpStr = 0;
		UInt32 uex = (UInt32)ex;
		while (true)
		{
			tmpStr -= 2;
			WriteNInt16((UInt8*)&tmpStr[0], ReadNInt16(&MyString_StrDigit100U8[(uex % 100) * 2]));
			uex = uex / 100;
			if (uex == 0)
				break;
		}
		if (*tmpStr == '0')
		{
			tmpStr++;
		}
		while ((*oriStr++ = *tmpStr++) != 0);
		oriStr--;
	}
	else if (ex < 0)
	{
		oriStr[0] = '0';
		oriStr[1] = '.';
		oriStr += 2;
		while (++ex < 0)
		{
			*oriStr++ = '0';
		}
		while (val > 1.0e-10)
		{
			val = val * 100.0;
			iVal = (Int32)val;
			val = val - iVal;
			WriteNInt16((UInt8*)&oriStr[0], ReadNInt16(&MyString_StrDigit100U8[iVal * 2]));
			oriStr += 2;
			if (--i <= 0)
				break;
		}
		while (oriStr[-1] == '0')
		{
			oriStr--;
		}
	}
	else
	{
		while (ex >= 1)
		{
			val = val * 100.0;
			iVal = (Int32)val;
			val = val - iVal;
			WriteNInt16((UInt8*)&oriStr[0], ReadNInt16(&MyString_StrDigit100U8[iVal * 2]));
			oriStr += 2;
			i--;
			ex -= 2;
		}
		if (ex == 0)
		{
			val = val * 100.0;
			iVal = (Int32)val;
			val = val - iVal;
			oriStr[0] = MyString_StrDigit100U8[iVal * 2];
			if (val > 1.0e-14)
			{
				oriStr[1] = '.';
				oriStr[2] = MyString_StrDigit100U8[iVal * 2 + 1];
				oriStr += 3;
			}
			else if (MyString_StrDigit100W[iVal * 2 + 1] != '0')
			{
				oriStr[1] = '.';
				oriStr[2] = MyString_StrDigit100U8[iVal * 2 + 1];
				oriStr += 3;
			}
			else
			{
				oriStr += 1;
			}
			i--;
		}
		else
		{
			if (val > 1.0e-10)
			{
				*oriStr++ = '.';
			}
		}
		if (val > 1.0e-10)
		{
			while (val > 1.0e-10)
			{
				val = val * 100.0;
				iVal = (Int32)val;
				val = val - iVal;
				WriteNInt16((UInt8*)&oriStr[0], ReadNInt16(&MyString_StrDigit100U8[iVal * 2]));
				oriStr += 2;
				if (--i <= 0)
					break;
			}
			while (oriStr[-1] == '0')
			{
				oriStr--;
			}
			if (oriStr[-1] == '.')
			{
				oriStr--;
			}
		}
	}
	*oriStr = 0;
	return oriStr;
}

UTF8Char *Text::StrDouble(UTF8Char *oriStr, Double val)
{
	return StrDouble(oriStr, val, 15, DoubleStyleExcel);
}

UTF16Char *Text::StrDouble(UTF16Char *oriStr, Double val)
{
	if (val == 0)
	{
		if (Math::IsNeg(val))
		{
			return Text::StrConcatASCII(oriStr, "-0");
		}
		else
		{
			return Text::StrConcatASCII(oriStr, "0");
		}
	}
	else if (Math::IsNAN(val))
	{
		return Text::StrConcatASCII(oriStr, "1.#QNAN0");
	}
	else if (val < 0)
	{
		*oriStr++ = '-';
		val = -val;
	}
	if (Math::IsInfinity(val))
	{
		return Text::StrConcatASCII(oriStr, "1.#INF00");
	}
	OSInt i = 7;
	Int32 ex = -10000 + (Int32)(Math_Log10(val) + 10000);
	Int32 iVal;
	val = val * Math_Pow(10.0, -ex - 1) + 5.0e-15;
	if (ex >= 16 || ex <= -4)
	{
		val = val * 100.0;
		iVal = (Int32)val;
		val = val - iVal;
		if (iVal < 0 || iVal >= 100) //Denom nunbers
		{
			return Text::StrConcatASCII(oriStr, "0");
		}
		oriStr[0] = MyString_StrDigit100U16[iVal * 2];
		oriStr[1] = '.';
		oriStr[2] = MyString_StrDigit100U16[iVal * 2 + 1];
		oriStr += 3;
		i--;

		if (val > 1.0e-10)
		{
			while (val > 1.0e-10)
			{
				val = val * 100.0;
				iVal = (Int32)val;
				val = val - iVal;
				WriteNInt32((UInt8*)&oriStr[0], ReadNInt32((const UInt8*)&MyString_StrDigit100U16[iVal * 2]));
				oriStr += 2;
				if (--i <= 0)
					break;
			}
			if (oriStr[-1] == '0')
			{
				oriStr -= 1;
			}
		}
		oriStr[0] = 'e';
		if (ex < 0)
		{
			oriStr[1] = '-';
			ex = -ex;
		}
		else
		{
			oriStr[1] = '+';
		}
		UTF16Char *tmpStr = oriStr + 13;
		oriStr += 2;
		*tmpStr = 0;
		UInt32 uex = (UInt32)ex;
		while (true)
		{
			tmpStr -= 2;
			WriteNInt32((UInt8*)&tmpStr[0], ReadNInt32((const UInt8*)&MyString_StrDigit100U16[(uex % 100) * 2]));
			uex = uex / 100;
			if (uex == 0)
				break;
		}
		if (*tmpStr == '0')
		{
			tmpStr++;
		}
		while ((*oriStr++ = *tmpStr++) != 0);
		oriStr--;
	}
	else if (ex < 0)
	{
		oriStr[0] = '0';
		oriStr[1] = '.';
		oriStr += 2;
		while (++ex < 0)
		{
			*oriStr++ = '0';
		}
		while (val > 1.0e-10)
		{
			val = val * 100.0;
			iVal = (Int32)val;
			val = val - iVal;
			WriteNInt32((UInt8*)&oriStr[0], ReadNInt32((const UInt8*)&MyString_StrDigit100U16[iVal * 2]));
			oriStr += 2;
			if (--i <= 0)
				break;
		}
		while (oriStr[-1] == '0')
		{
			oriStr--;
		}
	}
	else
	{
		while (ex >= 1)
		{
			val = val * 100.0;
			iVal = (Int32)val;
			val = val - iVal;
			WriteNInt32((UInt8*)&oriStr[0], ReadNInt32((const UInt8*)&MyString_StrDigit100U16[iVal * 2]));
			oriStr += 2;
			i--;
			ex -= 2;
		}
		if (ex == 0)
		{
			val = val * 100.0;
			iVal = (Int32)val;
			val = val - iVal;
			oriStr[0] = MyString_StrDigit100U16[iVal * 2];
			if (val > 1.0e-14)
			{
				oriStr[1] = '.';
				oriStr[2] = MyString_StrDigit100U16[iVal * 2 + 1];
				oriStr += 3;
			}
			else if (MyString_StrDigit100U16[iVal * 2 + 1] != '0')
			{
				oriStr[1] = '.';
				oriStr[2] = MyString_StrDigit100U16[iVal * 2 + 1];
				oriStr += 3;
			}
			else
			{
				oriStr += 1;
			}
			i--;
		}
		else
		{
			if (val > 1.0e-10)
			{
				*oriStr++ = '.';
			}
		}
		if (val > 1.0e-10)
		{
			while (val > 1.0e-10)
			{
				val = val * 100.0;
				iVal = (Int32)val;
				val = val - iVal;
				WriteNInt32((UInt8*)&oriStr[0], ReadNInt32((const UInt8*)&MyString_StrDigit100U16[iVal * 2]));
				oriStr += 2;
				if (--i <= 0)
					break;
			}
			while (oriStr[-1] == '0')
			{
				oriStr--;
			}
			if (oriStr[-1] == '.')
			{
				oriStr--;
			}
		}
	}
	*oriStr = 0;
	return oriStr;
}

UTF32Char *Text::StrDouble(UTF32Char *oriStr, Double val)
{
	if (val == 0)
	{
		if (Math::IsNeg(val))
		{
			return Text::StrConcatASCII(oriStr, "-0");
		}
		else
		{
			return Text::StrConcatASCII(oriStr, "0");
		}
	}
	else if (Math::IsNAN(val))
	{
		return Text::StrConcatASCII(oriStr, "1.#QNAN0");
	}
	else if (val < 0)
	{
		*oriStr++ = '-';
		val = -val;
	}
	if (Math::IsInfinity(val))
	{
		return Text::StrConcatASCII(oriStr, "1.#INF00");
	}
	OSInt i = 7;
	Int32 ex = -10000 + (Int32)(Math_Log10(val) + 10000);
	Int32 iVal;
	val = val * Math_Pow(10.0, -ex - 1) + 5.0e-15;
	if (ex >= 16 || ex <= -4)
	{
		val = val * 100.0;
		iVal = (Int32)val;
		val = val - iVal;
		oriStr[0] = MyString_StrDigit100U32[iVal * 2];
		oriStr[1] = '.';
		oriStr[2] = MyString_StrDigit100U32[iVal * 2 + 1];
		oriStr += 3;
		i--;

		if (val > 1.0e-10)
		{
			while (val > 1.0e-10)
			{
				val = val * 100.0;
				iVal = (Int32)val;
				val = val - iVal;
				WriteNInt64((UInt8*)&oriStr[0], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[iVal * 2]));
				oriStr += 2;
				if (--i <= 0)
					break;
			}
			if (oriStr[-1] == '0')
			{
				oriStr -= 1;
			}
		}
		oriStr[0] = 'e';
		if (ex < 0)
		{
			oriStr[1] = '-';
			ex = -ex;
		}
		else
		{
			oriStr[1] = '+';
		}
		UTF32Char *tmpStr = oriStr + 13;
		oriStr += 2;
		*tmpStr = 0;
		UInt32 uex = (UInt32)ex;
		while (true)
		{
			tmpStr -= 2;
			WriteNInt64((UInt8*)&tmpStr[0], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[(uex % 100) * 2]));
			uex = uex / 100;
			if (uex == 0)
				break;
		}
		if (*tmpStr == '0')
		{
			tmpStr++;
		}
		while ((*oriStr++ = *tmpStr++) != 0);
		oriStr--;
	}
	else if (ex < 0)
	{
		oriStr[0] = '0';
		oriStr[1] = '.';
		oriStr += 2;
		while (++ex < 0)
		{
			*oriStr++ = '0';
		}
		while (val > 1.0e-10)
		{
			val = val * 100.0;
			iVal = (Int32)val;
			val = val - iVal;
			WriteNInt64((UInt8*)&oriStr[0], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[iVal * 2]));
			oriStr += 2;
			if (--i <= 0)
				break;
		}
		while (oriStr[-1] == '0')
		{
			oriStr--;
		}
	}
	else
	{
		while (ex >= 1)
		{
			val = val * 100.0;
			iVal = (Int32)val;
			val = val - iVal;
			WriteNInt64((UInt8*)&oriStr[0], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[iVal * 2]));
			oriStr += 2;
			i--;
			ex -= 2;
		}
		if (ex == 0)
		{
			val = val * 100.0;
			iVal = (Int32)val;
			val = val - iVal;
			oriStr[0] = MyString_StrDigit100U32[iVal * 2];
			if (val > 1.0e-10)
			{
				oriStr[1] = '.';
				oriStr[2] = MyString_StrDigit100U32[iVal * 2 + 1];
				oriStr += 3;
			}
			else if (MyString_StrDigit100U32[iVal * 2 + 1] != '0')
			{
				oriStr[1] = '.';
				oriStr[2] = MyString_StrDigit100U32[iVal * 2 + 1];
				oriStr += 3;
			}
			else
			{
				oriStr += 1;
			}
			i--;
		}
		else
		{
			if (val > 1.0e-10)
			{
				*oriStr++ = '.';
			}
		}
		if (val > 1.0e-10)
		{
			while (val > 1.0e-10)
			{
				val = val * 100.0;
				iVal = (Int32)val;
				val = val - iVal;
				WriteNInt64((UInt8*)&oriStr[0], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[iVal * 2]));
				oriStr += 2;
				if (--i <= 0)
					break;
			}
			while (oriStr[-1] == '0')
			{
				oriStr--;
			}
			if (oriStr[-1] == '.')
			{
				oriStr--;
			}
		}
	}
	*oriStr = 0;
	return oriStr;
}

UTF8Char *Text::StrDoubleDP(UTF8Char *oriStr, Double val, UOSInt minDP, UOSInt maxDP)
{
	UTF8Char fmt[64];
	if (maxDP <= 0)
	{
		return StrDoubleFmt(oriStr, val, "0");
	}
	UTF8Char *sptr = Text::StrConcatC(fmt, UTF8STRC("0."));
	while (minDP > 0)
	{
		*sptr++ = '0';
		minDP--;
		maxDP--;
	}
	while (maxDP > 0)
	{
		*sptr++ = '#';
		maxDP--;
	}
	*sptr = 0;
	return StrDoubleFmt(oriStr, val, (const Char*)fmt);
}

UTF16Char *Text::StrDoubleDP(UTF16Char *oriStr, Double val, UOSInt minDP, UOSInt maxDP)
{
	Char fmt[64];
	if (maxDP <= 0)
	{
		return StrDoubleFmt(oriStr, val, "0");
	}
	Char *sptr = Text::StrConcatC(fmt, "0.", 2);
	while (minDP > 0)
	{
		*sptr++ = '0';
		minDP--;
		maxDP--;
	}
	while (maxDP > 0)
	{
		*sptr++ = '#';
		maxDP--;
	}
	*sptr = 0;
	return StrDoubleFmt(oriStr, val, fmt);
}

UTF32Char *Text::StrDoubleDP(UTF32Char *oriStr, Double val, UOSInt minDP, UOSInt maxDP)
{
	Char fmt[64];
	if (maxDP <= 0)
	{
		return StrDoubleFmt(oriStr, val, "0");
	}
	Char *sptr = Text::StrConcatC(fmt, "0.", 2);
	while (minDP > 0)
	{
		*sptr++ = '0';
		minDP--;
		maxDP--;
	}
	while (maxDP > 0)
	{
		*sptr++ = '#';
		maxDP--;
	}
	*sptr = 0;
	return StrDoubleFmt(oriStr, val, fmt);
}

Char *MyString_ecvt(Char *buff, Double val, Int32 numDigits, Int32 *digit, Int32 *sign)
{
	if (val < 0)
	{
		val = -val;
		*sign = 1;
	}
	else
	{
		*sign = 0;
	}
	Int32 ex;
	Int32 iVal;
	if (val == 0.0)
	{
		ex = 0;
	}
	else
	{
		ex = -10000 + (Int32)(Math_Log10(val) + 10000);
		val = val * Math_Pow(10.0, -ex - 1);
	}
	*digit = ex + 1;
	if (numDigits & 1)
	{
		numDigits >>= 1;
		while (numDigits > 0)
		{
			val = val * 100.0;
			iVal = (Int32)val;
			val = val - iVal;
			WriteNInt16((UInt8*)&buff[0], ReadNInt16(&MyString_StrDigit100U8[iVal * 2]));
			buff += 2;
			numDigits--;
		}
		val = val * 100.0;
		iVal = (Int32)val;
		buff[0] = (Char)MyString_StrDigit100U8[iVal * 2];
		buff++;
	}
	else
	{
		numDigits >>= 1;
		while (numDigits > 0)
		{
			val = val * 100.0;
			iVal = (Int32)val;
			val = val - iVal;
			WriteNInt16((UInt8*)&buff[0], ReadNInt16(&MyString_StrDigit100U8[iVal * 2]));
			buff += 2;
			numDigits--;
		}
	}
	*buff = 0;
	return buff;
}

UTF8Char *Text::StrDoubleFmt(UTF8Char *oriStr, Double val, const Char *format)
{
	Char fmtBuff[30];
	Char *buff;
	Char buff2[20];
	Char *buffPtr;
	Int32 digit;
	Int32 sign;
	Int32 percentMode = 0;
	Int32 emode = 0;
	Bool beginGroup = false;
	Bool beginDigit = false;
	Int32 groupCnt = 0;
	Int32 afterDigitS = 0;
	Int32 afterDigitZ = 0;
	Int32 beforeDigit = 0;
	Int32 eDigit = 0;
	OSInt leng;

	Char sbuff[256];
	Char *sptr;
	Char c;
	Char *sect[3];
	Int32 sectCnt;

	sectCnt = 1;
	sptr = sect[0] = sbuff;
	while (true)
	{
		c = *format++;
		if (c == 0)
		{
			*sptr++ = 0;
			break;
		}
		else if (c == '\\')
		{
			if (*format != 0)
			{
				*sptr++ = c;
				*sptr++ = *format++;
			}
			else
			{
				*sptr++ = c;
				*sptr++ = 0;
				break;
			}
		}
		else if (c == ';')
		{
			*sptr++ = 0;
			if (sectCnt == 3)
			{
				break;
			}
			else
			{
				sect[sectCnt++] = sptr;
			}
		}
		else
		{
			*sptr++ = c;
		}
	}

	if (sectCnt == 1)
	{
		format = sect[0];
	}
	else if (sectCnt == 2)
	{
		if (val >= 0)
		{
			format = sect[0];
		}
		else
		{
			format = sect[1];
		}
	}
	else
	{
		if (*sect[1] == 0)
		{
			sect[1] = sect[0];
		}
		if (val > 0)
		{
			format = sect[0];
		}
		else if (val < 0)
		{
			format = sect[1];
		}
		else
		{
			format = sect[2];
		}
	}
	leng = MyString_ecvt(fmtBuff, val, 20, &digit, &sign) - fmtBuff;
	buff = fmtBuff;
	while ((c = *format++) != 0)
	{
		if (c == '0' || c == '#' || c == '.' || c == ',' || c == '%')
			break;
		if (c == '\\')
		{
			if ((c = *format++) == 0)
			{
				*oriStr++ = '\\';
				break;
			}
			else
			{
				*oriStr++ = (UTF8Char)c;
			}
		}
		else
		{
			*oriStr++ = (UTF8Char)c;
		}
	}

	if (c == 0)
		return oriStr;

	if (c == '%')
	{
		percentMode = 2;
		c = *format++;
	}
	while (true)
	{
		if (c == 0)
		{
			break;
		}
		else if (c == ',')
		{
			beginGroup = true;
		}
		else if (c == '.')
		{
			beginDigit = true;
		}
		else if (c == 'e')
		{
			emode = 2;
			if (*format == '+')
			{
				emode |= 4;
				format++;
			}
		}
		else if (c == 'E')
		{
			emode = 1;
			if (*format == '+')
			{
				emode |= 4;
				format++;
			}
		}
		else if (c == '%')
		{
			percentMode = 1;
			format++;
			break;
		}
		else if (c == '#')
		{
			if (emode)
			{
			}
			else if (beginDigit)
			{
				afterDigitS++;
			}
			else if (beginGroup)
			{
				groupCnt++;
			}
		}
		else if (c == '0')
		{
			if (emode)
			{
				eDigit++;
			}
			else if (beginDigit)
			{
				afterDigitZ++;
			}
			else
			{
				beforeDigit++;
				if (beginGroup)
				{
					groupCnt++;
				}
			}
		}
		else
		{
			break;
		}
		c = *format++;
	}
	if (percentMode)
	{
		digit += 2;
		if (percentMode == 2)
		{
			*oriStr++ = '%';
		}
	}
	if (sign)
	{
		*oriStr++ = '-';
	}
	if (emode)
	{
		/////////////////////////////////////////////
		return oriStr;
	}
	else
	{
		Int32 maxLeng = afterDigitZ + afterDigitS + digit;
		Int32 minLeng = afterDigitZ + digit;

		if (maxLeng < 0)
		{
		}
		else if (minLeng < leng)
		{
			buffPtr = buff2;
			*buffPtr++ = '0';
			while (maxLeng-- > 0)
			{
				if ((c = *buffPtr++ = *buff++) == 0)
				{
					break;
				}
			}
			*buffPtr = 0;
			if (c != 0)
			{
				c = *buff;
				buff = buffPtr;
				if (c != 0)
				{
					if (c >= 0x35)
					{
						while (true)
						{
							c = ++*--buffPtr;
							if (c != 0x3a)
							{
								break;
							}
							*buffPtr = 0x30;
						}
					}
				}
				buffPtr = buff;
			}
			while (buffPtr > buff2 + 1 && (c = *--buffPtr) == '0')
			{
				*buffPtr = 0;
			}
			if (buff2[0] != '0')
			{
				digit++;
				buff = buff2;
			}
			else
			{
				buff = &buff2[1];
			}
		}
		
		if (digit <= 0)
		{
			if (groupCnt)
			{
				while (beforeDigit-- > 0)
				{
					*oriStr++ = '0';
					if (beforeDigit != 0 && (beforeDigit % groupCnt) == 0)
					{
						*oriStr++ = ',';
					}
				}
			}
			else
			{
				while (beforeDigit-- > 0)
				{
					*oriStr++ = '0';
				}
			}

		}
		else
		{
			if (groupCnt)
			{
				while (beforeDigit-- > digit)
				{
					*oriStr++ = '0';
					if (beforeDigit != 0 && (beforeDigit % groupCnt) == 0)
					{
						*oriStr++ = ',';
					}
				}

				while (digit > 0)
				{
					c = *buff;
					if (c == 0)
					{
						while (digit-- > 0)
						{
							*oriStr++ = '0';
							if (digit != 0 && (digit % groupCnt) == 0)
							{
								*oriStr++ = ',';
							}
						}
						break;
					}
					else
					{
						buff++;
						*oriStr++ = (UTF8Char)c;
						digit--;
						if (digit != 0 && (digit % groupCnt) == 0)
						{
							*oriStr++ = ',';
						}
					}
				}
			}
			else
			{
				while (beforeDigit-- > digit)
				{
					*oriStr++ = '0';
				}

				while (digit > 0)
				{
					c = *buff;
					if (c == 0)
					{
						while (digit-- > 0)
						{
							*oriStr++ = '0';
						}
						break;
					}
					else
					{
						buff++;
						*oriStr++ = (UTF8Char)c;
						digit--;
					}
				}
			}
		}

		if (afterDigitZ == 0 && (afterDigitS == 0 || *buff == 0))
		{
		}
		else
		{
			*oriStr++ = '.';

			while (digit < 0)
			{
				if (afterDigitZ > 0)
					afterDigitZ--;
				else if (afterDigitS > 0)
					afterDigitS--;
				else
					break;
				digit++;
				*oriStr++ = '0';
			}
			while (afterDigitZ > 0)
			{
				c = *buff;
				if (c == 0)
				{
					while (afterDigitZ-- > 0)
					{
						*oriStr++ = '0';
						break;
					}
				}
				else
				{
					buff++;
					afterDigitZ--;
					*oriStr++ = (UTF8Char)c;
				}
			}
			while (afterDigitS > 0)
			{
				c = *buff;
				if (c == 0)
					break;
				buff++;
				afterDigitS--;
				*oriStr++ = (UTF8Char)c;
			}
		}

		if (percentMode == 1)
		{
			*oriStr++ = '%';
		}
		return Text::StrConcat(oriStr, (const UTF8Char*)format - 1);
	}
}

UTF16Char *Text::StrDoubleFmt(UTF16Char *oriStr, Double val, const Char *format)
{
	Char fmtBuff[30];
	Char *buff;
	Char buff2[20];
	Char *buffPtr;
	Int32 digit;
	Int32 sign;
	Int32 percentMode = 0;
	Int32 emode = 0;
	Bool beginGroup = false;
	Bool beginDigit = false;
	Int32 groupCnt = 0;
	Int32 afterDigitS = 0;
	Int32 afterDigitZ = 0;
	Int32 beforeDigit = 0;
	Int32 eDigit = 0;
	OSInt leng;

	Char sbuff[256];
	Char *sptr;
	Char c;
	Char *sect[3];
	Int32 sectCnt;

	sectCnt = 1;
	sptr = sect[0] = sbuff;
	while (true)
	{
		c = *format++;
		if (c == 0)
		{
			*sptr++ = 0;
			break;
		}
		else if (c == '\\')
		{
			if (*format != 0)
			{
				*sptr++ = c;
				*sptr++ = *format++;
			}
			else
			{
				*sptr++ = c;
				*sptr++ = 0;
				break;
			}
		}
		else if (c == ';')
		{
			*sptr++ = 0;
			if (sectCnt == 3)
			{
				break;
			}
			else
			{
				sect[sectCnt++] = sptr;
			}
		}
		else
		{
			*sptr++ = c;
		}
	}

	if (sectCnt == 1)
	{
		format = sect[0];
	}
	else if (sectCnt == 2)
	{
		if (val >= 0)
		{
			format = sect[0];
		}
		else
		{
			format = sect[1];
		}
	}
	else
	{
		if (*sect[1] == 0)
		{
			sect[1] = sect[0];
		}
		if (val > 0)
		{
			format = sect[0];
		}
		else if (val < 0)
		{
			format = sect[1];
		}
		else
		{
			format = sect[2];
		}
	}
	leng = MyString_ecvt(fmtBuff, val, 20, &digit, &sign) - fmtBuff;
	buff = fmtBuff;
	while ((c = *format++) != 0)
	{
		if (c == '0' || c == '#' || c == '.' || c == ',' || c == '%')
			break;
		if (c == '\\')
		{
			if ((c = *format++) == 0)
			{
				*oriStr++ = '\\';
				break;
			}
			else
			{
				*oriStr++ = (UTF16Char)c;
			}
		}
		else
		{
			*oriStr++ = (UTF16Char)c;
		}
	}

	if (c == 0)
		return oriStr;

	if (c == '%')
	{
		percentMode = 2;
		c = *format++;
	}
	while (true)
	{
		if (c == 0)
		{
			break;
		}
		else if (c == ',')
		{
			beginGroup = true;
		}
		else if (c == '.')
		{
			beginDigit = true;
		}
		else if (c == 'e')
		{
			emode = 2;
			if (*format == '+')
			{
				emode |= 4;
				format++;
			}
		}
		else if (c == 'E')
		{
			emode = 1;
			if (*format == '+')
			{
				emode |= 4;
				format++;
			}
		}
		else if (c == '%')
		{
			percentMode = 1;
			format++;
			break;
		}
		else if (c == '#')
		{
			if (emode)
			{
			}
			else if (beginDigit)
			{
				afterDigitS++;
			}
			else if (beginGroup)
			{
				groupCnt++;
			}
		}
		else if (c == '0')
		{
			if (emode)
			{
				eDigit++;
			}
			else if (beginDigit)
			{
				afterDigitZ++;
			}
			else
			{
				beforeDigit++;
				if (beginGroup)
				{
					groupCnt++;
				}
			}
		}
		else
		{
			break;
		}
		c = *format++;
	}
	if (percentMode)
	{
		digit += 2;
		if (percentMode == 2)
		{
			*oriStr++ = '%';
		}
	}
	if (sign)
	{
		*oriStr++ = '-';
	}
	if (emode)
	{
		/////////////////////////////////////////////
		return oriStr;
	}
	else
	{
		Int32 maxLeng = afterDigitZ + afterDigitS + digit;
		Int32 minLeng = afterDigitZ + digit;

		if (maxLeng < 0)
		{
		}
		else if (minLeng < leng)
		{
			buffPtr = buff2;
			*buffPtr++ = '0';
			while (maxLeng-- > 0)
			{
				if ((c = *buffPtr++ = *buff++) == 0)
				{
					break;
				}
			}
			*buffPtr = 0;
			if (c != 0)
			{
				c = *buff;
				buff = buffPtr;
				if (c != 0)
				{
					if (c >= 0x35)
					{
						while (true)
						{
							c = ++*--buffPtr;
							if (c != 0x3a)
							{
								break;
							}
							*buffPtr = 0x30;
						}
					}
				}
				buffPtr = buff;
			}
			while (buffPtr > buff2 + 1 && (c = *--buffPtr) == '0')
			{
				*buffPtr = 0;
			}
			if (buff2[0] != '0')
			{
				digit++;
				buff = buff2;
			}
			else
			{
				buff = &buff2[1];
			}
		}
		
		if (digit <= 0)
		{
			if (groupCnt)
			{
				while (beforeDigit-- > 0)
				{
					*oriStr++ = '0';
					if (beforeDigit != 0 && (beforeDigit % groupCnt) == 0)
					{
						*oriStr++ = ',';
					}
				}
			}
			else
			{
				while (beforeDigit-- > 0)
				{
					*oriStr++ = '0';
				}
			}

		}
		else
		{
			if (groupCnt)
			{
				while (beforeDigit-- > digit)
				{
					*oriStr++ = '0';
					if (beforeDigit != 0 && (beforeDigit % groupCnt) == 0)
					{
						*oriStr++ = ',';
					}
				}

				while (digit > 0)
				{
					c = *buff;
					if (c == 0)
					{
						while (digit-- > 0)
						{
							*oriStr++ = '0';
							if (digit != 0 && (digit % groupCnt) == 0)
							{
								*oriStr++ = ',';
							}
						}
						break;
					}
					else
					{
						buff++;
						*oriStr++ = (UTF16Char)c;
						digit--;
						if (digit != 0 && (digit % groupCnt) == 0)
						{
							*oriStr++ = ',';
						}
					}
				}
			}
			else
			{
				while (beforeDigit-- > digit)
				{
					*oriStr++ = '0';
				}

				while (digit > 0)
				{
					c = *buff;
					if (c == 0)
					{
						while (digit-- > 0)
						{
							*oriStr++ = '0';
						}
						break;
					}
					else
					{
						buff++;
						*oriStr++ = (UTF16Char)c;
						digit--;
					}
				}
			}
		}

		if (afterDigitZ == 0 && (afterDigitS == 0 || *buff == 0))
		{
		}
		else
		{
			*oriStr++ = '.';

			while (digit < 0)
			{
				if (afterDigitZ > 0)
					afterDigitZ--;
				else if (afterDigitS > 0)
					afterDigitS--;
				else
					break;
				digit++;
				*oriStr++ = '0';
			}
			while (afterDigitZ > 0)
			{
				c = *buff;
				if (c == 0)
				{
					while (afterDigitZ-- > 0)
					{
						*oriStr++ = '0';
						break;
					}
				}
				else
				{
					buff++;
					afterDigitZ--;
					*oriStr++ = (UTF16Char)c;
				}
			}
			while (afterDigitS > 0)
			{
				c = *buff;
				if (c == 0)
					break;
				buff++;
				afterDigitS--;
				*oriStr++ = (UTF16Char)c;
			}
		}

		if (percentMode == 1)
		{
			*oriStr++ = '%';
		}
		return Text::StrConcatASCII(oriStr, format - 1);
	}
}

UTF32Char *Text::StrDoubleFmt(UTF32Char *oriStr, Double val, const Char *format)
{
	Char fmtBuff[30];
	Char *buff;
	Char buff2[20];
	Char *buffPtr;
	Int32 digit;
	Int32 sign;
	Int32 percentMode = 0;
	Int32 emode = 0;
	Bool beginGroup = false;
	Bool beginDigit = false;
	Int32 groupCnt = 0;
	Int32 afterDigitS = 0;
	Int32 afterDigitZ = 0;
	Int32 beforeDigit = 0;
	Int32 eDigit = 0;
	OSInt leng;

	Char sbuff[256];
	Char *sptr;
	Char c;
	Char *sect[3];
	Int32 sectCnt;

	sectCnt = 1;
	sptr = sect[0] = sbuff;
	while (true)
	{
		c = *format++;
		if (c == 0)
		{
			*sptr++ = 0;
			break;
		}
		else if (c == '\\')
		{
			if (*format != 0)
			{
				*sptr++ = c;
				*sptr++ = *format++;
			}
			else
			{
				*sptr++ = c;
				*sptr++ = 0;
				break;
			}
		}
		else if (c == ';')
		{
			*sptr++ = 0;
			if (sectCnt == 3)
			{
				break;
			}
			else
			{
				sect[sectCnt++] = sptr;
			}
		}
		else
		{
			*sptr++ = c;
		}
	}

	if (sectCnt == 1)
	{
		format = sect[0];
	}
	else if (sectCnt == 2)
	{
		if (val >= 0)
		{
			format = sect[0];
		}
		else
		{
			format = sect[1];
		}
	}
	else
	{
		if (*sect[1] == 0)
		{
			sect[1] = sect[0];
		}
		if (val > 0)
		{
			format = sect[0];
		}
		else if (val < 0)
		{
			format = sect[1];
		}
		else
		{
			format = sect[2];
		}
	}

	leng = MyString_ecvt(fmtBuff, val, 20, &digit, &sign) - fmtBuff;
	buff = fmtBuff;
	while ((c = *format++) != 0)
	{
		if (c == '0' || c == '#' || c == '.' || c == ',' || c == '%')
			break;
		if (c == '\\')
		{
			if ((c = *format++) == 0)
			{
				*oriStr++ = '\\';
				break;
			}
			else
			{
				*oriStr++ = (UTF32Char)c;
			}
		}
		else
		{
			*oriStr++ = (UTF32Char)c;
		}
	}

	if (c == 0)
		return oriStr;

	if (c == '%')
	{
		percentMode = 2;
		c = *format++;
	}
	while (true)
	{
		if (c == 0)
		{
			break;
		}
		else if (c == ',')
		{
			beginGroup = true;
		}
		else if (c == '.')
		{
			beginDigit = true;
		}
		else if (c == 'e')
		{
			emode = 2;
			if (*format == '+')
			{
				emode |= 4;
				format++;
			}
		}
		else if (c == 'E')
		{
			emode = 1;
			if (*format == '+')
			{
				emode |= 4;
				format++;
			}
		}
		else if (c == '%')
		{
			percentMode = 1;
			format++;
			break;
		}
		else if (c == '#')
		{
			if (emode)
			{
			}
			else if (beginDigit)
			{
				afterDigitS++;
			}
			else if (beginGroup)
			{
				groupCnt++;
			}
		}
		else if (c == '0')
		{
			if (emode)
			{
				eDigit++;
			}
			else if (beginDigit)
			{
				afterDigitZ++;
			}
			else
			{
				beforeDigit++;
				if (beginGroup)
				{
					groupCnt++;
				}
			}
		}
		else
		{
			break;
		}
		c = *format++;
	}
	if (percentMode)
	{
		digit += 2;
		if (percentMode == 2)
		{
			*oriStr++ = '%';
		}
	}
	if (sign)
	{
		*oriStr++ = '-';
	}
	if (emode)
	{
		/////////////////////////////////////////////
		return oriStr;
	}
	else
	{
		Int32 maxLeng = afterDigitZ + afterDigitS + digit;
		Int32 minLeng = afterDigitZ + digit;

		if (maxLeng < 0)
		{
		}
		else if (minLeng < leng)
		{
			buffPtr = buff2;
			*buffPtr++ = '0';
			while (maxLeng-- > 0)
			{
				if ((c = *buffPtr++ = *buff++) == 0)
				{
					break;
				}
			}
			*buffPtr = 0;
			if (c != 0)
			{
				c = *buff;
				buff = buffPtr;
				if (c != 0)
				{
					if (c >= 0x35)
					{
						while (true)
						{
							c = ++*--buffPtr;
							if (c != 0x3a)
							{
								break;
							}
							*buffPtr = 0x30;
						}
					}
				}
				buffPtr = buff;
			}
			while (buffPtr > buff2 + 1 && (c = *--buffPtr) == '0')
			{
				*buffPtr = 0;
			}
			if (buff2[0] != '0')
			{
				digit++;
				buff = buff2;
			}
			else
			{
				buff = &buff2[1];
			}
		}
		
		if (digit <= 0)
		{
			if (groupCnt)
			{
				while (beforeDigit-- > 0)
				{
					*oriStr++ = '0';
					if (beforeDigit != 0 && (beforeDigit % groupCnt) == 0)
					{
						*oriStr++ = ',';
					}
				}
			}
			else
			{
				while (beforeDigit-- > 0)
				{
					*oriStr++ = '0';
				}
			}

		}
		else
		{
			if (groupCnt)
			{
				while (beforeDigit-- > digit)
				{
					*oriStr++ = '0';
					if (beforeDigit != 0 && (beforeDigit % groupCnt) == 0)
					{
						*oriStr++ = ',';
					}
				}

				while (digit > 0)
				{
					c = *buff;
					if (c == 0)
					{
						while (digit-- > 0)
						{
							*oriStr++ = '0';
							if (digit != 0 && (digit % groupCnt) == 0)
							{
								*oriStr++ = ',';
							}
						}
						break;
					}
					else
					{
						buff++;
						*oriStr++ = (UTF32Char)c;
						digit--;
						if (digit != 0 && (digit % groupCnt) == 0)
						{
							*oriStr++ = ',';
						}
					}
				}
			}
			else
			{
				while (beforeDigit-- > digit)
				{
					*oriStr++ = '0';
				}

				while (digit > 0)
				{
					c = *buff;
					if (c == 0)
					{
						while (digit-- > 0)
						{
							*oriStr++ = '0';
						}
						break;
					}
					else
					{
						buff++;
						*oriStr++ = (UTF32Char)c;
						digit--;
					}
				}
			}
		}

		if (afterDigitZ == 0 && (afterDigitS == 0 || *buff == 0))
		{
		}
		else
		{
			*oriStr++ = '.';

			while (digit < 0)
			{
				if (afterDigitZ > 0)
					afterDigitZ--;
				else if (afterDigitS > 0)
					afterDigitS--;
				else
					break;
				digit++;
				*oriStr++ = '0';
			}
			while (afterDigitZ > 0)
			{
				c = *buff;
				if (c == 0)
				{
					while (afterDigitZ-- > 0)
					{
						*oriStr++ = '0';
						break;
					}
				}
				else
				{
					buff++;
					afterDigitZ--;
					*oriStr++ = (UTF32Char)c;
				}
			}
			while (afterDigitS > 0)
			{
				c = *buff;
				if (c == 0)
					break;
				buff++;
				afterDigitS--;
				*oriStr++ = (UTF32Char)c;
			}
		}

		if (percentMode == 1)
		{
			*oriStr++ = '%';
		}
		return Text::StrConcatASCII(oriStr, format - 1);
	}
}

Bool Text::StrToDouble(const UTF8Char *str1, OutParam<Double> outVal)
{
	Bool neg = false;
	UTF8Char c;
	if (*str1 == '-')
	{
		neg = true;
		++str1;
	}
#if _OSINT_SIZE == 64
	Double r;
	Int64 ir = 0;
	while (true)
	{
		c = *str1++;
		if (c < '0' || c > '9')
			break;
		ir = ir * 10 + (c - '0');
	}
	if (c == '.')
	{
		Double fmul = 1;
        while (true)
		{
			c = *str1++;
			if (c < '0' || c > '9')
				break;
			fmul *= 0.1;
			ir = ir * 10 + (c - '0');
        }
		r = (Double)ir * fmul;
    }
	else if (c == 0)
	{
		if (neg)
		{
			outVal.Set(-(Double)ir);
			return true;
		}
		else
		{
			outVal.Set((Double)ir);
			return true;
		}
	}
#else
	Double r = 0.0;
	while (true)
	{
		c = *str1++;
		if (c < '0' || c > '9')
			break;
		r = (r * 10.0) + (c - '0');
	}
	if (c == '.')
	{
		Double fmul = 1;
        while (true)
		{
			c = *str1++;
			if (c < '0' || c > '9')
				break;
			fmul *= 0.1;
			r += (c - '0') * fmul;
        }
    }
	else if (c == 0)
	{
		if (neg)
		{
			r = -r;
		}
		outVal.Set(r);
		return true;
	}
#endif
	else
	{
		return false;
	}
	if (c == 'e' || c == 'E')
	{
		c = *str1++;
		Bool eneg;
		Int32 expV;
		if (c == '+')
		{
			eneg = false;
		}
		else if (c == '-')
		{
			eneg = true;
		}
		else
		{
			str1--;
			eneg = false;
		}
		expV = 0;
		while (true)
		{
			c = *str1++;
			if (c < '0' || c > '9')
				break;
			expV = expV * 10 + c - '0';
		}
		if (eneg)
		{
			r = r * Math_Pow(10.0, -expV);
		}
		else
		{
			r = r * Math_Pow(10.0, expV);
		}
	}
	if (c != 0)
	{
		return false;
	}
    if (neg)
	{
        r = -r;
    }
	outVal.Set(r);
	return true;
}

Bool Text::StrToDouble(const UTF16Char *str1, OutParam<Double> outVal)
{
	Double r = 0.0;
	Bool neg = false;
	UTF16Char c;
	if (*str1 == '-')
	{
		neg = true;
		++str1;
	}
	while (true)
	{
		c = *str1++;
		if (c < '0' || c > '9')
			break;
		r = (r * 10.0) + (c - '0');
	}
	if (c == '.')
	{
		Double fmul = 1;
        while (true)
		{
			c = *str1++;
			if (c < '0' || c > '9')
				break;
			fmul *= 0.1;
			r += (c - '0') * fmul;
        }
    }
	else if (c == 0)
	{
		if (neg)
		{
			outVal.Set(-r);
			return true;
		}
		else
		{
			outVal.Set(r);
			return true;
		}
	}
	else
	{
		return false;
	}
	if (c == 'e' || c == 'E')
	{
		c = *str1++;
		Bool eneg;
		Int32 expV;
		if (c == '+')
		{
			eneg = false;
		}
		else if (c == '-')
		{
			eneg = true;
		}
		else
		{
			str1--;
			eneg = false;
		}
		expV = 0;
		while (true)
		{
			c = *str1++;
			if (c < '0' || c > '9')
				break;
			expV = expV * 10 + c - '0';
		}
		if (eneg)
		{
			r = r * Math_Pow(10.0, -expV);
		}
		else
		{
			r = r * Math_Pow(10.0, expV);
		}
	}
	if (c != 0)
	{
		return false;
	}
    if (neg)
	{
        r = -r;
    }
	outVal.Set(r);
	return true;
}

Bool Text::StrToDouble(const UTF32Char *str1, OutParam<Double> outVal)
{
	Double r = 0.0;
	Bool neg = false;
	UTF32Char c;
	if (*str1 == '-')
	{
		neg = true;
		++str1;
	}
	while (true)
	{
		c = *str1++;
		if (c < '0' || c > '9')
			break;
		r = (r * 10.0) + (c - '0');
	}
	if (c == '.')
	{
		Double fmul = 1;
        while (true)
		{
			c = *str1++;
			if (c < '0' || c > '9')
				break;
			fmul *= 0.1;
			r += (c - '0') * fmul;
        }
    }
	else if (c == 0)
	{
		if (neg)
		{
			outVal.Set(-r);
			return true;
		}
		else
		{
			outVal.Set(r);
			return true;
		}
	}
	else
	{
		return false;
	}
	if (c == 'e' || c == 'E')
	{
		c = *str1++;
		Bool eneg;
		Int32 expV;
		if (c == '+')
		{
			eneg = false;
		}
		else if (c == '-')
		{
			eneg = true;
		}
		else
		{
			str1--;
			eneg = false;
		}
		expV = 0;
		while (true)
		{
			c = *str1++;
			if (c < '0' || c > '9')
				break;
			expV = expV * 10 + (Int32)(c - '0');
		}
		if (eneg)
		{
			r = r * Math_Pow(10.0, -expV);
		}
		else
		{
			r = r * Math_Pow(10.0, expV);
		}
	}
	if (c != 0)
	{
		return false;
	}
    if (neg)
	{
        outVal.Set(-r);
    }
	else
	{
		outVal.Set(r);
	}
	return true;
}

Double Text::StrToDouble(const UTF8Char *str1)
{
	Double r;
	if (Text::StrToDouble(str1, r))
		return r;
	return 0;
}

Double Text::StrToDouble(const UTF16Char *str1)
{
	Double r;
	if (Text::StrToDouble(str1, r))
		return r;
	return 0;
}

Double Text::StrToDouble(const UTF32Char *str1)
{
	Double r;
	if (Text::StrToDouble(str1, r))
		return r;
	return 0;
}

void Text::SBAppendF32(Text::StringBuilderUTF *sb, Single v)
{
	UTF8Char sbuff[33];
	sb->AppendC(sbuff, (UOSInt)(Text::StrDouble(sbuff, v) - sbuff));
}

void Text::SBAppendF64(Text::StringBuilderUTF *sb, Double v)
{
	UTF8Char sbuff[33];
	sb->AppendC(sbuff, (UOSInt)(Text::StrDouble(sbuff, v) - sbuff));
}
