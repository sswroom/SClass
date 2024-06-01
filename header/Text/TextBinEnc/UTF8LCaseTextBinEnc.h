#ifndef _SM_TEXT_TEXTBINENC_UTF8LCASETEXTBINENC
#define _SM_TEXT_TEXTBINENC_UTF8LCASETEXTBINENC
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class UTF8LCaseTextBinEnc : public Text::TextBinEnc::ITextBinEnc
		{
		public:
			UTF8LCaseTextBinEnc();
			virtual ~UTF8LCaseTextBinEnc();

			virtual UOSInt EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UOSInt buffSize) const;
			virtual UOSInt CalcBinSize(Text::CStringNN str) const;
			virtual UOSInt DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
