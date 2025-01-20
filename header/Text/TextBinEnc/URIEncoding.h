#ifndef _SM_TEXT_TEXTENC_URIENCODING
#define _SM_TEXT_TEXTENC_URIENCODING
#include "Text/TextBinEnc/TextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class URIEncoding : public Text::TextBinEnc::TextBinEnc
		{
		public:
			static UnsafeArray<UTF8Char> URIEncode(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> uri);
			static UnsafeArray<UTF8Char> URIDecode(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> uri);
			
			URIEncoding();
			virtual ~URIEncoding();
			virtual UOSInt EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UOSInt buffSize) const;
			virtual UOSInt CalcBinSize(Text::CStringNN str) const;
			virtual UOSInt DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
