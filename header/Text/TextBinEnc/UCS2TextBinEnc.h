#ifndef _SM_TEXT_TEXTBINENC_UCS2TEXTBINENC
#define _SM_TEXT_TEXTBINENC_UCS2TEXTBINENC
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class UCS2TextBinEnc : public Text::TextBinEnc::ITextBinEnc
		{
		public:
			UCS2TextBinEnc();
			virtual ~UCS2TextBinEnc();
			virtual UOSInt EncodeBin(NotNullPtr<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize);
			virtual UOSInt CalcBinSize(const UTF8Char *str, UOSInt strLen);
			virtual UOSInt DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff);
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
