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

		PString()
		{
		}

		PString(UTF8Char *v, UOSInt leng)
		{
			this->v = v;
			this->leng = leng;
		}

		Text::CString ToCString()
		{
			return {this->v, this->leng};
		}

		Text::PString Substring(UOSInt index)
		{
			return {this->v + index, this->leng - index};
		}

		Text::PString TrimAsNew()
		{
			UTF8Char *sptr = this->v;
			UTF8Char *endPtr = this->v + this->leng;
			while (sptr < endPtr && (*sptr == ' ' || *sptr == '\t'))
			{
				sptr++;
			}
			while (sptr < endPtr && (endPtr[-1] == ' ' || endPtr[-1] == '\t'))
			{
				endPtr--;
			}
			*endPtr = 0;
			return {sptr, (UOSInt)(endPtr - sptr)};
		}

		Text::PString SubstrTrim(UOSInt index)
		{
			if (index >= this->leng)
			{
				return {this->v, 0};
			}
			UTF8Char *sptr = this->v + index;
			UTF8Char *endPtr = this->v + this->leng;
			while (sptr < endPtr && (*sptr == ' ' || *sptr == '\t'))
			{
				sptr++;
			}
			while (sptr < endPtr && (endPtr[-1] == ' ' || endPtr[-1] == '\t'))
			{
				endPtr--;
			}
			*endPtr = 0;
			return {sptr, (UOSInt)(endPtr - sptr)};
		}
	};

	UOSInt StrSplitP(PString *strs, UOSInt maxStrs, PString strToSplit, UTF8Char splitChar); //Optimized
	UOSInt StrSplitTrimP(PString *strs, UOSInt maxStrs, PString strToSplit, UTF8Char splitChar); //Optimized
	UOSInt StrSplitLineP(PString *strs, UOSInt maxStrs, PString strToSplit); //Optimized
	UOSInt StrSplitWSP(PString *strs, UOSInt maxStrs, PString strToSplit); //Optimized
	UOSInt StrCSVSplitP(Text::PString *strs, UOSInt maxStrs, UTF8Char *strToSplit);
}
#endif
