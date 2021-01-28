#ifndef _SM_TEXT_TEXTBINENC_BASE64ENC
#define _SM_TEXT_TEXTBINENC_BASE64ENC
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class Base64Enc : public Text::TextBinEnc::ITextBinEnc
		{
		public:
			Base64Enc();
			virtual ~Base64Enc();
			virtual UOSInt EncodeBin(Text::StringBuilderUTF *sb, const UInt8 *dataBuff, UOSInt buffSize);
			virtual UOSInt CalcBinSize(const UTF8Char *b64Str);
			virtual UOSInt CalcBinSize(const WChar *sbuff);
			virtual UOSInt DecodeBin(const UTF8Char *b64Str, UInt8 *dataBuff);
			UOSInt DecodeBin(const UTF8Char *b64Str, UOSInt len, UInt8 *dataBuff);
			virtual UOSInt DecodeBin(const WChar *sbuff, UInt8 *dataBuff);
			virtual const UTF8Char *GetName();
		};
	}
}
#endif
