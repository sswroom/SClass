#ifndef _SM_TEXT_TEXTBINENC_ASCII85ENC
#define _SM_TEXT_TEXTBINENC_ASCII85ENC
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class ASCII85Enc : public Text::TextBinEnc::ITextBinEnc
		{
		public:
			ASCII85Enc();
			virtual ~ASCII85Enc();
			virtual UOSInt EncodeBin(NotNullPtr<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize);
			UTF8Char *EncodeBin(UTF8Char *sbuff, const UInt8 *dataBuff, UOSInt buffSize);
			virtual UOSInt CalcBinSize(const UTF8Char *a85Str, UOSInt strLen);
			virtual UOSInt DecodeBin(const UTF8Char *a85Str, UOSInt strLen, UInt8 *dataBuff);
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
