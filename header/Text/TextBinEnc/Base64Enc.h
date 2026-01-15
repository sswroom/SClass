#ifndef _SM_TEXT_TEXTBINENC_BASE64ENC
#define _SM_TEXT_TEXTBINENC_BASE64ENC
#include "Text/TextBinEnc/TextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class Base64Enc : public Text::TextBinEnc::TextBinEnc
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
			virtual UIntOS EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UIntOS buffSize) const;
			UIntOS EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UIntOS buffSize, Text::LineBreakType lbt, UIntOS charsPerLine) const;
			UnsafeArray<UTF8Char> EncodeBin(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UInt8> dataBuff, UIntOS buffSize);
			virtual UIntOS CalcBinSize(Text::CStringNN b64Str) const;
			UIntOS CalcBinSize(UnsafeArray<const WChar> sbuff) const;
			virtual UIntOS DecodeBin(Text::CStringNN b64Str, UnsafeArray<UInt8> dataBuff) const;
			UIntOS DecodeBin(UnsafeArray<const WChar> sbuff, UnsafeArray<UInt8> dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
