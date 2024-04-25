#ifndef _SM_TEXT_MYSTRINGFLOAT
#define _SM_TEXT_MYSTRINGFLOAT
#ifndef SDEFS_INCLUDED
#include "Stdafx.h"
#endif

namespace Text
{
	struct DoubleStyle
	{
		const UTF8Char *infStr;
		UOSInt infLen;
		const UTF8Char *nanStr;
		UOSInt nanLen;	
	};

	static const DoubleStyle DoubleStyleExcel = {UTF8STRC("1.#INF00"), UTF8STRC("1.#QNAN0")};
	static const DoubleStyle DoubleStyleC = {UTF8STRC("INF"), UTF8STRC("NAN")};

	UTF8Char *StrDouble(UTF8Char *oriStr, Double val, UOSInt sigFig, NN<const DoubleStyle> style);
	UTF8Char *StrDouble(UTF8Char *oriStr, Double val);
	FORCEINLINE Char *StrDouble(Char *oriStr, Double val) { return (Char*)StrDouble((UTF8Char*)oriStr, val); }
	UTF16Char *StrDouble(UTF16Char *oriStr, Double val);
	UTF32Char *StrDouble(UTF32Char *oriStr, Double val);

	UTF8Char *StrDoubleDP(UTF8Char *oriStr, Double val, UOSInt minDP, UOSInt maxDP);
	FORCEINLINE Char *StrDoubleDP(Char *oriStr, Double val, UOSInt minDP, UOSInt maxDP) { return (Char*)StrDoubleDP((UTF8Char*)oriStr, val, minDP, maxDP); }
	UTF16Char *StrDoubleDP(UTF16Char *oriStr, Double val, UOSInt minDP, UOSInt maxDP);
	UTF32Char *StrDoubleDP(UTF32Char *oriStr, Double val, UOSInt minDP, UOSInt maxDP);

	UTF8Char *StrDoubleFmt(UTF8Char *oriStr, Double val, const Char *format);
	FORCEINLINE Char *StrDoubleFmt(Char *oriStr, Double val, const Char *format) { return (Char*)StrDoubleFmt((UTF8Char*)oriStr, val, format); }
	UTF16Char *StrDoubleFmt(UTF16Char *oriStr, Double val, const Char *format);
	UTF32Char *StrDoubleFmt(UTF32Char *oriStr, Double val, const Char *format);

	Bool StrToDouble(const UTF8Char *str, OutParam<Double> outVal);
	FORCEINLINE Bool StrToDouble(const Char *str, OutParam<Double> outVal) { return StrToDouble((const UTF8Char*)str, outVal); }
	Bool StrToDouble(const UTF16Char *str, OutParam<Double> outVal);
	Bool StrToDouble(const UTF32Char *str, OutParam<Double> outVal);
	Double StrToDouble(const UTF8Char *str);
	FORCEINLINE Double StrToDouble(const Char *str) { return StrToDouble((const UTF8Char*)str); }
	Double StrToDouble(const UTF16Char *str);
	Double StrToDouble(const UTF32Char *str);

	class StringBuilderUTF;
	void SBAppendF32(Text::StringBuilderUTF *sb, Single v);
	void SBAppendF64(Text::StringBuilderUTF *sb, Double v);
}
#endif
