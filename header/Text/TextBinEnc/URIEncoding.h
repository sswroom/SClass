#ifndef _SM_TEXT_TEXTENC_URIENCODING
#define _SM_TEXT_TEXTENC_URIENCODING
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class URIEncoding : public Text::TextBinEnc::ITextBinEnc
		{
		public:
			static UTF8Char *URIEncode(UTF8Char *buff, const UTF8Char *uri);
			static UTF8Char *URIDecode(UTF8Char *buff, const UTF8Char *uri);
			
			URIEncoding();
			virtual ~URIEncoding();
			virtual UOSInt EncodeBin(NN<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize) const;
			virtual UOSInt CalcBinSize(Text::CStringNN str) const;
			virtual UOSInt DecodeBin(Text::CStringNN str, UInt8 *dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
