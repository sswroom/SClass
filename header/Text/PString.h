#ifndef _SM_TEXT_PSTRING
#define _SM_TEXT_PSTRING
#include "Text/CString.h"
namespace Text
{
	struct PString : public StringBase<UTF8Char>
	{
		void Trim() { this->leng = (UOSInt)(Text::StrTrimC(this->v, this->leng) - this->v); }
		void TrimSp() { this->leng = (UOSInt)(Text::StrTrimSpC(this->v, this->leng) - this->v); }
		void Trim(UOSInt index);
		void RTrim();
		void TrimWSCRLF();
		void ToUpper();
		void ToLower();
		void ToCapital();
		UOSInt Replace(UTF8Char fromChar, UTF8Char toChar);
		void RemoveWS();

		PString() = default;

		PString(UnsafeArray<UTF8Char> v, UOSInt leng)
		{
			this->v = v;
			this->leng = leng;
		}

		Text::CStringNN ToCString() const
		{
			if (this->v.Ptr())
				return Text::CStringNN(this->v, this->leng);
			else
				return CSTR("");
		}

		Text::PString Substring(UOSInt index) const
		{
			return {this->v + index, this->leng - index};
		}

		Text::PString TrimAsNew()
		{
			UnsafeArray<UTF8Char> sptr = this->v;
			UnsafeArray<UTF8Char> endPtr = this->v + this->leng;
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
				return {this->v + this->leng, 0};
			}
			UnsafeArray<UTF8Char> sptr = this->v + index;
			UnsafeArray<UTF8Char> endPtr = this->v + this->leng;
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

		Text::PString SubstrTrim(UOSInt index, UOSInt leng)
		{
			if (index >= this->leng)
			{
				return {this->v + this->leng, 0};
			}
			UnsafeArray<UTF8Char> sptr = this->v + index;
			UnsafeArray<UTF8Char> endPtr;
			if (index + leng >= this->leng)
			{
				endPtr = this->v + this->leng;
			}
			else
			{
				endPtr = this->v + index + leng;
			}
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

		void TrimToLength(UOSInt newLen)
		{
			if (newLen < this->leng)
			{
				this->leng = newLen;
				this->v[this->leng] = 0;
			}
		}

		void RemoveChars(UOSInt cnt)
		{
			if (cnt >= this->leng)
			{
				this->leng = 0;
				this->v[0] = 0;
			}
			else
			{
				this->leng -= cnt;
				this->v[this->leng] = 0;
			}
		}

		void RemoveChars(UOSInt index, UOSInt cnt)
		{
			UOSInt endOfst = index + cnt;
			if (endOfst >= this->leng)
			{
				this->TrimToLength(index); 
			}
			else
			{
				this->leng = (UOSInt)(Text::StrConcatC(this->v + index, this->v + endOfst, this->leng - endOfst) - this->v);
			}
		}
	};

	UOSInt StrSplitP(UnsafeArray<PString> strs, UOSInt maxStrs, PString strToSplit, UTF8Char splitChar); //Optimized
	UOSInt StrSplitTrimP(UnsafeArray<PString> strs, UOSInt maxStrs, PString strToSplit, UTF8Char splitChar); //Optimized
	UOSInt StrSplitLineP(UnsafeArray<PString> strs, UOSInt maxStrs, PString strToSplit); //Optimized
	UOSInt StrSplitWSP(UnsafeArray<PString> strs, UOSInt maxStrs, PString strToSplit); //Optimized
	UOSInt StrCSVSplitP(UnsafeArray<Text::PString> strs, UOSInt maxStrs, UnsafeArray<UTF8Char> strToSplit);
}
#endif
