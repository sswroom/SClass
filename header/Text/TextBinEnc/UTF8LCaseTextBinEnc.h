#ifndef _SM_TEXT_TEXTBINENC_UTF8LCASETEXTBINENC
#define _SM_TEXT_TEXTBINENC_UTF8LCASETEXTBINENC
#include "Text/TextBinEnc/TextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class UTF8LCaseTextBinEnc : public Text::TextBinEnc::TextBinEnc
		{
		public:
			UTF8LCaseTextBinEnc();
			virtual ~UTF8LCaseTextBinEnc();

			virtual UIntOS EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UIntOS buffSize) const;
			virtual UIntOS CalcBinSize(Text::CStringNN str) const;
			virtual UIntOS DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
