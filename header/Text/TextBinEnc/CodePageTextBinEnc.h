#ifndef _SM_TEXT_TEXTBINENC_CODEPAGETEXTBINENC
#define _SM_TEXT_TEXTBINENC_CODEPAGETEXTBINENC
#include "Text/Encoding.h"
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class CodePageTextBinEnc : public Text::TextBinEnc::ITextBinEnc
		{
		private:
			Text::Encoding *enc;
		public:
			CodePageTextBinEnc(UInt32 codePage);
			virtual ~CodePageTextBinEnc();
			virtual UOSInt EncodeBin(NotNullPtr<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize);
			virtual UOSInt CalcBinSize(const UTF8Char *str, UOSInt strLen);
			virtual UOSInt DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff);
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
