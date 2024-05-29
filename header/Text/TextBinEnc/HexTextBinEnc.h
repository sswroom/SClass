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
			virtual UOSInt EncodeBin(NN<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize) const;
			virtual UOSInt CalcBinSize(Text::CStringNN str) const;
			virtual UOSInt DecodeBin(Text::CStringNN str, UInt8 *dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
