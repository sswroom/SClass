#ifndef _SM_TEXT_TEXTBINENC_RADIX64ENC
#define _SM_TEXT_TEXTBINENC_RADIX64ENC
#include "Text/TextBinEnc/TextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class Radix64Enc : public Text::TextBinEnc::TextBinEnc
		{
		private:
			UInt8 decArr[256];
			Char encArr[64];
		public:
			Radix64Enc(const Char *endArr);
			virtual ~Radix64Enc();
			virtual UOSInt EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UOSInt buffSize) const;
			virtual UOSInt CalcBinSize(Text::CStringNN str) const;
			virtual UOSInt CalcBinSize(const WChar *sbuff) const;
			virtual UOSInt DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const;
			virtual UOSInt DecodeBin(const WChar *sbuff, UnsafeArray<UInt8> dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
