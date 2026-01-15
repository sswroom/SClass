#ifndef _SM_TEXT_PSTRING
#define _SM_TEXT_PSTRING
#include "Text/CString.h"
namespace Text
{
	struct PString : public StringBase<UTF8Char>
	{
		void Trim() { this->leng = (UIntOS)(Text::StrTrimC(this->v, this->leng) - this->v); }
		void TrimSp() { this->leng = (UIntOS)(Text::StrTrimSpC(this->v, this->leng) - this->v); }
		void Trim(UIntOS index);
		void RTrim();
		void TrimWSCRLF();
		void ToUpper();
		void ToLower();
		void ToCapital();
		UIntOS Replace(UTF8Char fromChar, UTF8Char toChar);
		void RemoveWS();
		void RemoveChar(UTF8Char c);

		PString() = default;

		PString(UnsafeArray<UTF8Char> v, UIntOS leng)
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

		Text::PString Substring(UIntOS index) const
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
			return {sptr, (UIntOS)(endPtr - sptr)};
		}

		Text::PString SubstrTrim(UIntOS index)
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
			return {sptr, (UIntOS)(endPtr - sptr)};
		}

		Text::PString SubstrTrim(UIntOS index, UIntOS leng)
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
			return {sptr, (UIntOS)(endPtr - sptr)};
		}

		void TrimToLength(UIntOS newLen)
		{
			if (newLen < this->leng)
			{
				this->leng = newLen;
				this->v[this->leng] = 0;
			}
		}

		void RemoveChars(UIntOS cnt)
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

		void RemoveChars(UIntOS index, UIntOS cnt)
		{
			UIntOS endOfst = index + cnt;
			if (endOfst >= this->leng)
			{
				this->TrimToLength(index); 
			}
			else
			{
				this->leng = (UIntOS)(Text::StrConcatC(this->v + index, this->v + endOfst, this->leng - endOfst) - this->v);
			}
		}
	};

	UIntOS StrSplitP(UnsafeArray<PString> strs, UIntOS maxStrs, PString strToSplit, UTF8Char splitChar); //Optimized
	UIntOS StrSplitTrimP(UnsafeArray<PString> strs, UIntOS maxStrs, PString strToSplit, UTF8Char splitChar); //Optimized
	UIntOS StrSplitLineP(UnsafeArray<PString> strs, UIntOS maxStrs, PString strToSplit); //Optimized
	UIntOS StrSplitWSP(UnsafeArray<PString> strs, UIntOS maxStrs, PString strToSplit); //Optimized
	UIntOS StrCSVSplitP(UnsafeArray<Text::PString> strs, UIntOS maxStrs, UnsafeArray<UTF8Char> strToSplit);
}
#endif
