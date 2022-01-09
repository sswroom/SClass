#ifndef _SM_TEXT_TEXTBINENC_BASE32ENC
#define _SM_TEXT_TEXTBINENC_BASE32ENC
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class Base32Enc : public Text::TextBinEnc::ITextBinEnc
		{
		private:
			static const UInt8 decArr[];
			static const UTF8Char *GetEncArr();
		public:
			Base32Enc();
			virtual ~Base32Enc();
			virtual UOSInt EncodeBin(Text::StringBuilderUTF *sb, const UInt8 *dataBuff, UOSInt buffSize);
			virtual UOSInt CalcBinSize(const UTF8Char *str, UOSInt strLen);
			virtual UOSInt CalcBinSize(const WChar *sbuff);
			virtual UOSInt DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff);
			virtual const UTF8Char *GetName();

			static Bool IsValid(const UTF8Char *b32Str);
		};
	}
}
#endif
