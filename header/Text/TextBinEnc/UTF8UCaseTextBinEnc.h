#ifndef _SM_TEXT_TEXTBINENC_UTF8UCASETEXTBINENC
#define _SM_TEXT_TEXTBINENC_UTF8UCASETEXTBINENC
#include "Text/TextBinEnc/TextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class UTF8UCaseTextBinEnc : public Text::TextBinEnc::TextBinEnc
		{
		public:
			UTF8UCaseTextBinEnc();
			virtual ~UTF8UCaseTextBinEnc();

			virtual UIntOS EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UIntOS buffSize) const;
			virtual UIntOS CalcBinSize(Text::CStringNN str) const;
			virtual UIntOS DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
