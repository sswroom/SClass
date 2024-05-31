#ifndef _SM_TEXT_TEXTBINENC_BASE64ENC
#define _SM_TEXT_TEXTBINENC_BASE64ENC
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class Base64Enc : public Text::TextBinEnc::ITextBinEnc
		{
		public:
			enum class Charset
			{
				Normal,
				URL
			};
		private:
			Charset cs;
			Bool noPadding;
			static const UInt8 decArr[];
			static UnsafeArray<const UTF8Char> GetEncArr(Charset cs);
		public:
			Base64Enc();
			Base64Enc(Charset cs, Bool noPadding);
			virtual ~Base64Enc();
			virtual UOSInt EncodeBin(NN<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize) const;
			UOSInt EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UOSInt buffSize, Text::LineBreakType lbt, UOSInt charsPerLine) const;
			UnsafeArray<UTF8Char> EncodeBin(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UInt8> dataBuff, UOSInt buffSize);
			virtual UOSInt CalcBinSize(Text::CStringNN b64Str) const;
			virtual UOSInt CalcBinSize(const WChar *sbuff) const;
			virtual UOSInt DecodeBin(Text::CStringNN b64Str, UInt8 *dataBuff) const;
			virtual UOSInt DecodeBin(const WChar *sbuff, UInt8 *dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
