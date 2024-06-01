#ifndef _SM_TEXT_TEXTBINENC_CPPTEXTBINENC
#define _SM_TEXT_TEXTBINENC_CPPTEXTBINENC
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class CPPTextBinEnc : public Text::TextBinEnc::ITextBinEnc
		{
		public:
			CPPTextBinEnc();
			virtual ~CPPTextBinEnc();
			virtual UOSInt EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UOSInt buffSize) const;
			virtual UOSInt CalcBinSize(Text::CStringNN str) const;
			virtual UOSInt DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
