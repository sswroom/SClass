#ifndef _SM_TEXT_TEXTBINENC_ASN1OIDBINENC
#define _SM_TEXT_TEXTBINENC_ASN1OIDBINENC
#include "Text/CString.h"
#include "Text/TextBinEnc/TextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class ASN1OIDBinEnc : public Text::TextBinEnc::TextBinEnc
		{
		public:
			ASN1OIDBinEnc();
			virtual ~ASN1OIDBinEnc();
			virtual UOSInt EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UOSInt buffSize) const;
			virtual UOSInt CalcBinSize(Text::CStringNN str) const;
			virtual UOSInt DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
