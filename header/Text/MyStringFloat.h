#ifndef _SM_TEXT_MYSTRINGFLOAT
#define _SM_TEXT_MYSTRINGFLOAT
#ifndef SDEFS_INCLUDED
#include "Stdafx.h"
#endif

namespace Text
{
	struct DoubleStyle
	{
		UnsafeArray<const UTF8Char> infStr;
		UOSInt infLen;
		UnsafeArray<const UTF8Char> nanStr;
		UOSInt nanLen;	
	};

	static const DoubleStyle DoubleStyleExcel = {UTF8STRC("1.#INF00"), UTF8STRC("1.#QNAN0")};
	static const DoubleStyle DoubleStyleC = {UTF8STRC("INF"), UTF8STRC("NAN")};

	UnsafeArray<UTF8Char> StrDouble(UnsafeArray<UTF8Char> oriStr, Double val, UOSInt sigFig, NN<const DoubleStyle> style);
	UnsafeArray<UTF8Char> StrDouble(UnsafeArray<UTF8Char> oriStr, Double val);
	FORCEINLINE UnsafeArray<Char> StrDoubleCh(UnsafeArray<Char> oriStr, Double val) { return UnsafeArray<Char>::ConvertFrom(StrDouble(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val)); }
	UTF16Char *StrDoubleW(UTF16Char *oriStr, Double val);
	UTF32Char *StrDoubleW(UTF32Char *oriStr, Double val);

	UnsafeArray<UTF8Char> StrDoubleDP(UnsafeArray<UTF8Char> oriStr, Double val, UOSInt minDP, UOSInt maxDP);
	FORCEINLINE UnsafeArray<Char> StrDoubleDPCh(UnsafeArray<Char> oriStr, Double val, UOSInt minDP, UOSInt maxDP) { return UnsafeArray<Char>::ConvertFrom(StrDoubleDP(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val, minDP, maxDP)); }
	UTF16Char *StrDoubleDPW(UTF16Char *oriStr, Double val, UOSInt minDP, UOSInt maxDP);
	UTF32Char *StrDoubleDPW(UTF32Char *oriStr, Double val, UOSInt minDP, UOSInt maxDP);

	UnsafeArray<UTF8Char> StrDoubleFmt(UnsafeArray<UTF8Char> oriStr, Double val, const Char *format);
	FORCEINLINE UnsafeArray<Char> StrDoubleFmtCh(UnsafeArray<Char> oriStr, Double val, const Char *format) { return UnsafeArray<Char>::ConvertFrom(StrDoubleFmt(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val, format)); }
	UTF16Char *StrDoubleFmtW(UTF16Char *oriStr, Double val, const Char *format);
	UTF32Char *StrDoubleFmtW(UTF32Char *oriStr, Double val, const Char *format);

	Bool StrToDouble(UnsafeArray<const UTF8Char> str, OutParam<Double> outVal);
	FORCEINLINE Bool StrToDoubleCh(UnsafeArray<const Char> str, OutParam<Double> outVal) { return StrToDouble(UnsafeArray<const UTF8Char>::ConvertFrom(str), outVal); }
	Bool StrToDoubleW(const UTF16Char *str, OutParam<Double> outVal);
	Bool StrToDoubleW(const UTF32Char *str, OutParam<Double> outVal);
	Double StrToDouble(UnsafeArray<const UTF8Char> str);
	FORCEINLINE Double StrToDoubleCh(UnsafeArray<const Char> str) { return StrToDouble(UnsafeArray<const UTF8Char>::ConvertFrom(str)); }
	Double StrToDoubleW(const UTF16Char *str);
	Double StrToDoubleW(const UTF32Char *str);

	class StringBuilderUTF;
	void SBAppendF32(Text::StringBuilderUTF *sb, Single v);
	void SBAppendF64(Text::StringBuilderUTF *sb, Double v);
}
#endif
