#ifndef _SM_TEXT_TEXTENC_PUNYCODE
#define _SM_TEXT_TEXTENC_PUNYCODE
#include "Text/CString.h"
#include "Text/TextBinEnc/TextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class Punycode : public Text::TextBinEnc::TextBinEnc
		{
		private:
			static UIntOS Adapt(UIntOS delta, UIntOS numPoints, Bool firstTime);
		public:
			static UnsafeArray<UTF8Char> Encode(UnsafeArray<UTF8Char> buff, Text::CStringNN strToEnc);
			static UnsafeArray<UTF8Char> Encode(UnsafeArray<UTF8Char> buff, const WChar *strToEnc);
			static UnsafeArray<WChar> Encode(UnsafeArray<WChar> buff, UnsafeArray<const WChar> strToEnc);
			static UnsafeArray<UTF8Char> Decode(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> strToDec, UIntOS strLen);
			static WChar *Decode(WChar *buff, UnsafeArray<const UTF8Char> strToDec);
			static WChar *Decode(WChar *buff, const WChar *strToDec);
			
			Punycode();
			virtual ~Punycode();
			UnsafeArray<UTF8Char> EncodeString(UnsafeArray<UTF8Char> buff, const WChar *strToEnc) const;
			virtual UIntOS EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UIntOS buffSize) const;
			virtual UIntOS CalcBinSize(Text::CStringNN str) const;
			virtual UIntOS DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
