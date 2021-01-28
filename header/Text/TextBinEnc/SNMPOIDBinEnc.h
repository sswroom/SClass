#ifndef _SM_TEXT_TEXTBINENC_SNMPOIDBINENC
#define _SM_TEXT_TEXTBINENC_SNMPOIDBINENC
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class SNMPOIDBinEnc : public Text::TextBinEnc::ITextBinEnc
		{
		public:
			SNMPOIDBinEnc();
			virtual ~SNMPOIDBinEnc();
			virtual UOSInt EncodeBin(Text::StringBuilderUTF *sb, const UInt8 *dataBuff, UOSInt buffSize);
			virtual UOSInt CalcBinSize(const UTF8Char *b64Str);
			virtual UOSInt DecodeBin(const UTF8Char *b64Str, UInt8 *dataBuff);
			virtual const UTF8Char *GetName();
		};
	}
}
#endif
