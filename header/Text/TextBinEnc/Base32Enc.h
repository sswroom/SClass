#ifndef _SM_TEXT_TEXTBINENC_BASE32ENC
#define _SM_TEXT_TEXTBINENC_BASE32ENC
#include "Text/TextBinEnc/TextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class Base32Enc : public Text::TextBinEnc::TextBinEnc
		{
		private:
			static const UInt8 decArr[];
			static UnsafeArray<const UTF8Char> GetEncArr();
		public:
			Base32Enc();
			virtual ~Base32Enc();
			virtual UOSInt EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UOSInt buffSize) const;
			virtual UOSInt CalcBinSize(Text::CStringNN str) const;
			virtual UOSInt CalcBinSize(const WChar *sbuff) const;
			virtual UOSInt DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const;
			virtual Text::CStringNN GetName() const;

			static Bool IsValid(UnsafeArray<const UTF8Char> b32Str);
		};
	}
}
#endif
