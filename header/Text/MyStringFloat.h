#ifndef _SM_TEXT_MYSTRINGFLOAT
#define _SM_TEXT_MYSTRINGFLOAT
#ifndef SDEFS_INCLUDED
#include "Stdafx.h"
#endif
#include "Math/Math_C.h"

namespace Text
{
	struct DoubleStyle
	{
		UnsafeArray<const UTF8Char> infStr;
		UIntOS infLen;
		UnsafeArray<const UTF8Char> nanStr;
		UIntOS nanLen;	
	};

	static const DoubleStyle DoubleStyleExcel = {UTF8STRC("1.#INF00"), UTF8STRC("1.#QNAN0")};
	static const DoubleStyle DoubleStyleC = {UTF8STRC("INF"), UTF8STRC("NAN")};

	UnsafeArray<UTF8Char> StrDouble(UnsafeArray<UTF8Char> oriStr, Double val, UIntOS sigFig, NN<const DoubleStyle> style);
	UnsafeArray<UTF8Char> StrDouble(UnsafeArray<UTF8Char> oriStr, Double val);
	FORCEINLINE UnsafeArray<Char> StrDoubleCh(UnsafeArray<Char> oriStr, Double val) { return UnsafeArray<Char>::ConvertFrom(StrDouble(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val)); }
	UnsafeArray<UTF16Char> StrDoubleW(UnsafeArray<UTF16Char> oriStr, Double val);
	UnsafeArray<UTF32Char> StrDoubleW(UnsafeArray<UTF32Char> oriStr, Double val);

	UnsafeArray<UTF8Char> StrDoubleDP(UnsafeArray<UTF8Char> oriStr, Double val, UIntOS minDP, UIntOS maxDP);
	FORCEINLINE UnsafeArray<Char> StrDoubleDPCh(UnsafeArray<Char> oriStr, Double val, UIntOS minDP, UIntOS maxDP) { return UnsafeArray<Char>::ConvertFrom(StrDoubleDP(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val, minDP, maxDP)); }
	UnsafeArray<UTF16Char> StrDoubleDPW(UnsafeArray<UTF16Char> oriStr, Double val, UIntOS minDP, UIntOS maxDP);
	UnsafeArray<UTF32Char> StrDoubleDPW(UnsafeArray<UTF32Char> oriStr, Double val, UIntOS minDP, UIntOS maxDP);
	UnsafeArray<UTF8Char> StrDoubleGDP(UnsafeArray<UTF8Char> oriStr, Double val, UIntOS groupCnt, UIntOS minDP, UIntOS maxDP);

	UnsafeArray<UTF8Char> StrDoubleFmt(UnsafeArray<UTF8Char> oriStr, Double val, UnsafeArray<const Char> format);
	FORCEINLINE UnsafeArray<Char> StrDoubleFmtCh(UnsafeArray<Char> oriStr, Double val, UnsafeArray<const Char> format) { return UnsafeArray<Char>::ConvertFrom(StrDoubleFmt(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val, format)); }
	UnsafeArray<UTF16Char> StrDoubleFmtW(UnsafeArray<UTF16Char> oriStr, Double val, UnsafeArray<const Char> format);
	UnsafeArray<UTF32Char> StrDoubleFmtW(UnsafeArray<UTF32Char> oriStr, Double val, UnsafeArray<const Char> format);

	Bool StrToDouble(UnsafeArray<const UTF8Char> str, OutParam<Double> outVal);
	FORCEINLINE Bool StrToDoubleCh(UnsafeArray<const Char> str, OutParam<Double> outVal) { return StrToDouble(UnsafeArray<const UTF8Char>::ConvertFrom(str), outVal); }
	Bool StrToDoubleW(UnsafeArray<const UTF16Char> str, OutParam<Double> outVal);
	Bool StrToDoubleW(UnsafeArray<const UTF32Char> str, OutParam<Double> outVal);
	FORCEINLINE Double StrToDoubleOr(UnsafeArray<const UTF8Char> str, Double v) { Double ret; return StrToDouble(str, ret)?ret:v; }
	FORCEINLINE Double StrToDoubleOrNAN(UnsafeArray<const UTF8Char> str) { Double ret; return StrToDouble(str, ret)?ret:NAN; }
	FORCEINLINE Double StrToDoubleOrCh(UnsafeArray<const Char> str, Double v) { return StrToDoubleOr(UnsafeArray<const UTF8Char>::ConvertFrom(str), v); }
	FORCEINLINE Double StrToDoubleOrNANCh(UnsafeArray<const Char> str) { return StrToDoubleOrNAN(UnsafeArray<const UTF8Char>::ConvertFrom(str)); }
	FORCEINLINE Double StrToDoubleOrW(UnsafeArray<const UTF16Char> str, Double v) { Double ret; return StrToDoubleW(str, ret)?ret:v;};
	FORCEINLINE Double StrToDoubleOrW(UnsafeArray<const UTF32Char> str, Double v) { Double ret; return StrToDoubleW(str, ret)?ret:v;};

	class StringBuilderUTF;
	void SBAppendF32(NN<Text::StringBuilderUTF> sb, Single v);
	void SBAppendF64(NN<Text::StringBuilderUTF> sb, Double v);
}
#endif
