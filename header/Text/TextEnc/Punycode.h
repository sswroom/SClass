#ifndef _SM_TEXT_TEXTENC_PUNYCODE
#define _SM_TEXT_TEXTENC_PUNYCODE
#include "Text/TextEncoding.h"

namespace Text
{
	namespace TextEnc
	{
		class Punycode : public Text::TextEncoding
		{
		private:
			static UOSInt Adapt(UOSInt delta, UOSInt numPoints, Bool firstTime);
		public:
			static UTF8Char *Encode(UTF8Char *buff, const UTF8Char *strToEnc);
			static UTF8Char *Encode(UTF8Char *buff, const WChar *strToEnc);
			static WChar *Encode(WChar *buff, const WChar *strToEnc);
			static UTF8Char *Decode(UTF8Char *buff, const UTF8Char *strToDec);
			static WChar *Decode(WChar *buff, const UTF8Char *strToDec);
			static WChar *Decode(WChar *buff, const WChar *strToDec);
			
			Punycode();
			virtual ~Punycode();
			UTF8Char *EncodeString(UTF8Char *buff, const WChar *strToEnc);
			virtual UTF8Char *EncodeString(UTF8Char *buff, const UTF8Char *strToEnc);
			virtual UTF8Char *DecodeString(UTF8Char *buff, const UTF8Char *strToDec);
		};
	}
}
#endif
