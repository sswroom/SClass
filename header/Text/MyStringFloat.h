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
	UnsafeArray<UTF16Char> StrDoubleW(UnsafeArray<UTF16Char> oriStr, Double val);
	UnsafeArray<UTF32Char> StrDoubleW(UnsafeArray<UTF32Char> oriStr, Double val);

	UnsafeArray<UTF8Char> StrDoubleDP(UnsafeArray<UTF8Char> oriStr, Double val, UOSInt minDP, UOSInt maxDP);
	FORCEINLINE UnsafeArray<Char> StrDoubleDPCh(UnsafeArray<Char> oriStr, Double val, UOSInt minDP, UOSInt maxDP) { return UnsafeArray<Char>::ConvertFrom(StrDoubleDP(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val, minDP, maxDP)); }
	UnsafeArray<UTF16Char> StrDoubleDPW(UnsafeArray<UTF16Char> oriStr, Double val, UOSInt minDP, UOSInt maxDP);
	UnsafeArray<UTF32Char> StrDoubleDPW(UnsafeArray<UTF32Char> oriStr, Double val, UOSInt minDP, UOSInt maxDP);

	UnsafeArray<UTF8Char> StrDoubleFmt(UnsafeArray<UTF8Char> oriStr, Double val, UnsafeArray<const Char> format);
	FORCEINLINE UnsafeArray<Char> StrDoubleFmtCh(UnsafeArray<Char> oriStr, Double val, UnsafeArray<const Char> format) { return UnsafeArray<Char>::ConvertFrom(StrDoubleFmt(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val, format)); }
	UnsafeArray<UTF16Char> StrDoubleFmtW(UnsafeArray<UTF16Char> oriStr, Double val, UnsafeArray<const Char> format);
	UnsafeArray<UTF32Char> StrDoubleFmtW(UnsafeArray<UTF32Char> oriStr, Double val, UnsafeArray<const Char> format);

	Bool StrToDouble(UnsafeArray<const UTF8Char> str, OutParam<Double> outVal);
	FORCEINLINE Bool StrToDoubleCh(UnsafeArray<const Char> str, OutParam<Double> outVal) { return StrToDouble(UnsafeArray<const UTF8Char>::ConvertFrom(str), outVal); }
	Bool StrToDoubleW(UnsafeArray<const UTF16Char> str, OutParam<Double> outVal);
	Bool StrToDoubleW(UnsafeArray<const UTF32Char> str, OutParam<Double> outVal);
	Double StrToDouble(UnsafeArray<const UTF8Char> str);
	FORCEINLINE Double StrToDoubleCh(UnsafeArray<const Char> str) { return StrToDouble(UnsafeArray<const UTF8Char>::ConvertFrom(str)); }
	Double StrToDoubleW(UnsafeArray<const UTF16Char> str);
	Double StrToDoubleW(UnsafeArray<const UTF32Char> str);

	class StringBuilderUTF;
	void SBAppendF32(NN<Text::StringBuilderUTF> sb, Single v);
	void SBAppendF64(NN<Text::StringBuilderUTF> sb, Double v);
}
#endif
