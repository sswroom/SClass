#ifndef _SM_TEXT_TEXTBINENC_UTF8LCASETEXTBINENC
#define _SM_TEXT_TEXTBINENC_UTF8LCASETEXTBINENC
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class UTF8LCaseTextBinEnc : public Text::TextBinEnc::ITextBinEnc
		{
		public:
			UTF8LCaseTextBinEnc();
			virtual ~UTF8LCaseTextBinEnc();

			virtual UOSInt EncodeBin(Text::StringBuilderUTF *sb, const UInt8 *dataBuff, UOSInt buffSize);
			virtual UOSInt CalcBinSize(const UTF8Char *str, UOSInt strLen);
			virtual UOSInt DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff);
			virtual Text::CString GetName();
		};
	}
}
#endif