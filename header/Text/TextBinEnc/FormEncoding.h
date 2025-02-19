
#ifndef _SM_TEXT_TEXTENC_FORMENCODING
#define _SM_TEXT_TEXTENC_FORMENCODING
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/TextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class FormEncoding : public Text::TextBinEnc::TextBinEnc
		{
		private:
			static UInt8 URIAllow[];
		public:
			static void FormEncode(NN<Text::StringBuilderUTF8> sb, Text::CStringNN uri);
			static void FormDecode(NN<Text::StringBuilderUTF8> sb, Text::CStringNN uri);
			static UnsafeArray<UTF8Char> FormEncode(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> uri);
			static UnsafeArray<UTF8Char> FormDecode(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> uri);
			
			FormEncoding();
			virtual ~FormEncoding();
			virtual UOSInt EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UOSInt buffSize) const;
			virtual UOSInt CalcBinSize(Text::CStringNN str) const;
			virtual UOSInt DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};

	}
}
#endif
