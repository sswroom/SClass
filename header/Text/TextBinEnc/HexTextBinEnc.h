#ifndef _SM_TEXT_TEXTBINENC_HEXTEXTBINENC
#define _SM_TEXT_TEXTBINENC_HEXTEXTBINENC
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class HexTextBinEnc : public Text::TextBinEnc::ITextBinEnc
		{
		public:
			HexTextBinEnc();
			virtual ~HexTextBinEnc();
			virtual UOSInt EncodeBin(NotNullPtr<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize);
			virtual UOSInt CalcBinSize(const UTF8Char *str, UOSInt strLen);
			virtual UOSInt DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff);
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
