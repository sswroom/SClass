#ifndef _SM_TEXT_PSTRING
#define _SM_TEXT_PSTRING
#include "Text/CString.h"
namespace Text
{
	struct PString : public StringBase<UTF8Char>
	{
		void Trim();
		void Trim(UOSInt index);
		void RTrim();
		void TrimWSCRLF();
		void TrimToLength(UOSInt newLen);
		void RemoveChars(UOSInt cnt);
		void RemoveChars(UOSInt index, UOSInt cnt);
		void ToUpper();
		void ToLower();
		void ToCapital();
		UOSInt Replace(UTF8Char fromChar, UTF8Char toChar);

		Text::CString ToCString()
		{
			return {this->v, this->leng};
		}
	};

	UOSInt StrSplitP(PString *strs, UOSInt maxStrs, UTF8Char *str, UOSInt strLen, UTF8Char splitChar); //Optimized
	UOSInt StrSplitTrimP(PString *strs, UOSInt maxStrs, UTF8Char *str, UOSInt strLen, UTF8Char splitChar); //Optimized
	UOSInt StrSplitLineP(PString *strs, UOSInt maxStrs, UTF8Char *str, UOSInt strLen); //Optimized
	UOSInt StrSplitWSP(PString *strs, UOSInt maxStrs, UTF8Char *str, UOSInt strLen); //Optimized
	UOSInt StrCSVSplitP(Text::PString *strs, UOSInt maxStrs, UTF8Char *strToSplit);
}
#endif
