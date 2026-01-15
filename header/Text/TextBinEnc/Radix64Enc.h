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
			virtual UIntOS EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UIntOS buffSize) const;
			virtual UIntOS CalcBinSize(Text::CStringNN str) const;
			virtual UIntOS CalcBinSize(const WChar *sbuff) const;
			virtual UIntOS DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const;
			virtual UIntOS DecodeBin(const WChar *sbuff, UnsafeArray<UInt8> dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
