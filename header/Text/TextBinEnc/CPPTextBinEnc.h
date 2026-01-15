#ifndef _SM_TEXT_TEXTBINENC_CPPTEXTBINENC
#define _SM_TEXT_TEXTBINENC_CPPTEXTBINENC
#include "Text/TextBinEnc/TextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class CPPTextBinEnc : public Text::TextBinEnc::TextBinEnc
		{
		public:
			CPPTextBinEnc();
			virtual ~CPPTextBinEnc();
			virtual UIntOS EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UIntOS buffSize) const;
			virtual UIntOS CalcBinSize(Text::CStringNN str) const;
			virtual UIntOS DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
