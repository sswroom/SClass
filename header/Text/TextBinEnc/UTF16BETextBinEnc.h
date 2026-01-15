#ifndef _SM_TEXT_TEXTBINENC_UTF16BETEXTBINENC
#define _SM_TEXT_TEXTBINENC_UTF16BETEXTBINENC
#include "Text/TextBinEnc/TextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class UTF16BETextBinEnc : public Text::TextBinEnc::TextBinEnc
		{
		public:
			UTF16BETextBinEnc();
			virtual ~UTF16BETextBinEnc();

			virtual UIntOS EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UIntOS buffSize) const;
			virtual UIntOS CalcBinSize(Text::CStringNN str) const;
			virtual UIntOS DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
