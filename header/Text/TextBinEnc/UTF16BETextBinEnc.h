#ifndef _SM_TEXT_TEXTBINENC_UTF16BETEXTBINENC
#define _SM_TEXT_TEXTBINENC_UTF16BETEXTBINENC
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class UTF16BETextBinEnc : public Text::TextBinEnc::ITextBinEnc
		{
		public:
			UTF16BETextBinEnc();
			virtual ~UTF16BETextBinEnc();

			virtual UOSInt EncodeBin(NN<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize);
			virtual UOSInt CalcBinSize(const UTF8Char *str, UOSInt strLen);
			virtual UOSInt DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff);
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
