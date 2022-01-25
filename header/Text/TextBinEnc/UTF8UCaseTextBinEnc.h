#ifndef _SM_TEXT_TEXTBINENC_UTF8UCASETEXTBINENC
#define _SM_TEXT_TEXTBINENC_UTF8UCASETEXTBINENC
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class UTF8UCaseTextBinEnc : public Text::TextBinEnc::ITextBinEnc
		{
		public:
			UTF8UCaseTextBinEnc();
			virtual ~UTF8UCaseTextBinEnc();

			virtual UOSInt EncodeBin(Text::StringBuilderUTF8 *sb, const UInt8 *dataBuff, UOSInt buffSize);
			virtual UOSInt CalcBinSize(const UTF8Char *str, UOSInt strLen);
			virtual UOSInt DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff);
			virtual Text::CString GetName();
		};
	}
}
#endif
