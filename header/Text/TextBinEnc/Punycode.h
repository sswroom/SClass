#ifndef _SM_TEXT_TEXTENC_PUNYCODE
#define _SM_TEXT_TEXTENC_PUNYCODE
#include "Text/CString.h"
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class Punycode : public Text::TextBinEnc::ITextBinEnc
		{
		private:
			static UOSInt Adapt(UOSInt delta, UOSInt numPoints, Bool firstTime);
		public:
			static UTF8Char *Encode(UTF8Char *buff, Text::CString strToEnc);
			static UTF8Char *Encode(UTF8Char *buff, const WChar *strToEnc);
			static WChar *Encode(WChar *buff, const WChar *strToEnc);
			static UTF8Char *Decode(UTF8Char *buff, const UTF8Char *strToDec, UOSInt strLen);
			static WChar *Decode(WChar *buff, const UTF8Char *strToDec);
			static WChar *Decode(WChar *buff, const WChar *strToDec);
			
			Punycode();
			virtual ~Punycode();
			UTF8Char *EncodeString(UTF8Char *buff, const WChar *strToEnc);
			virtual UOSInt EncodeBin(NotNullPtr<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize);
			virtual UOSInt CalcBinSize(const UTF8Char *str, UOSInt strLen);
			virtual UOSInt DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff);
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
