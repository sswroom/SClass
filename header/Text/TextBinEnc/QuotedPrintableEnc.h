#ifndef _SM_TEXT_TEXTBINENC_QUOTEDPRINTABLEENC
#define _SM_TEXT_TEXTBINENC_QUOTEDPRINTABLEENC
#include "Text/TextBinEnc/TextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class QuotedPrintableEnc : public Text::TextBinEnc::TextBinEnc
		{
		public:
			QuotedPrintableEnc();
			virtual ~QuotedPrintableEnc();
			virtual UIntOS EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UIntOS buffSize) const;
			virtual UIntOS CalcBinSize(Text::CStringNN str) const;
			virtual UIntOS DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
