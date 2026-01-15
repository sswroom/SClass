#ifndef _SM_TEXT_TEXTBINENC_CODEPAGETEXTBINENC
#define _SM_TEXT_TEXTBINENC_CODEPAGETEXTBINENC
#include "Text/Encoding.h"
#include "Text/TextBinEnc/TextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class CodePageTextBinEnc : public Text::TextBinEnc::TextBinEnc
		{
		private:
			Text::Encoding *enc;
		public:
			CodePageTextBinEnc(UInt32 codePage);
			virtual ~CodePageTextBinEnc();
			virtual UIntOS EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UIntOS buffSize) const;
			virtual UIntOS CalcBinSize(Text::CStringNN str) const;
			virtual UIntOS DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
