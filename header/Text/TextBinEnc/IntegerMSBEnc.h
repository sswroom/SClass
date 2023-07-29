#ifndef _SM_TEXT_TEXTBINENC_INTEGERMSBENC
#define _SM_TEXT_TEXTBINENC_INTEGERMSBENC
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class IntegerMSBEnc : public Text::TextBinEnc::ITextBinEnc
		{
		public:
			IntegerMSBEnc();
			virtual ~IntegerMSBEnc();
			virtual UOSInt EncodeBin(NotNullPtr<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize);
			virtual UOSInt CalcBinSize(const UTF8Char *str, UOSInt strLen);
			virtual UOSInt DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff);
			virtual Text::CString GetName();
		};
	}
}
#endif
