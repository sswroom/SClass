#ifndef _SM_TEXT_TEXTBINENC_ASN1OIDBINENC
#define _SM_TEXT_TEXTBINENC_ASN1OIDBINENC
#include "Text/CString.h"
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class ASN1OIDBinEnc : public Text::TextBinEnc::ITextBinEnc
		{
		public:
			ASN1OIDBinEnc();
			virtual ~ASN1OIDBinEnc();
			virtual UOSInt EncodeBin(Text::StringBuilderUTF8 *sb, const UInt8 *dataBuff, UOSInt buffSize);
			virtual UOSInt CalcBinSize(const UTF8Char *str, UOSInt strLen);
			virtual UOSInt DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff);
			virtual Text::CString GetName();
		};
	}
}
#endif
