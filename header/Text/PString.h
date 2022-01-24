#ifndef _SM_TEXT_PSTRING
#define _SM_TEXT_PSTRING
#include "Text/StringBase.h"
namespace Text
{
	struct PString : public StringBase<UTF8Char>
	{
		void RTrim();
	};

	UOSInt StrSplitP(PString *strs, UOSInt maxStrs, UTF8Char *str, UOSInt strLen, UTF8Char splitChar); //Optimized
	UOSInt StrSplitTrimP(PString *strs, UOSInt maxStrs, UTF8Char *str, UOSInt strLen, UTF8Char splitChar); //Optimized
	UOSInt StrSplitLineP(PString *strs, UOSInt maxStrs, UTF8Char *str, UOSInt strLen); //Optimized
	UOSInt StrSplitWSP(PString *strs, UOSInt maxStrs, UTF8Char *str, UOSInt strLen); //Optimized
	UOSInt StrCSVSplitP(Text::PString *strs, UOSInt maxStrs, UTF8Char *strToSplit);
}
#endif
