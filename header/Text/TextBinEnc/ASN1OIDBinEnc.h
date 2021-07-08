#ifndef _SM_TEXT_TEXTBINENC_ASN1OIDBINENC
#define _SM_TEXT_TEXTBINENC_ASN1OIDBINENC
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
			virtual UOSInt EncodeBin(Text::StringBuilderUTF *sb, const UInt8 *dataBuff, UOSInt buffSize);
			virtual UOSInt CalcBinSize(const UTF8Char *b64Str);
			virtual UOSInt DecodeBin(const UTF8Char *b64Str, UInt8 *dataBuff);
			virtual const UTF8Char *GetName();
		};
	}
}
#endif
