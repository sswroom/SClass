#ifndef _SM_TEXT_TEXTBINENC_UCS2TEXTBINENC
#define _SM_TEXT_TEXTBINENC_UCS2TEXTBINENC
#include "Text/TextBinEnc/TextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class UCS2TextBinEnc : public Text::TextBinEnc::TextBinEnc
		{
		public:
			UCS2TextBinEnc();
			virtual ~UCS2TextBinEnc();
			virtual UIntOS EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UIntOS buffSize) const;
			virtual UIntOS CalcBinSize(Text::CStringNN str) const;
			virtual UIntOS DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
