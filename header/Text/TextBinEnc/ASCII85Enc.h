#ifndef _SM_TEXT_TEXTBINENC_ASCII85ENC
#define _SM_TEXT_TEXTBINENC_ASCII85ENC
#include "Text/TextBinEnc/TextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class ASCII85Enc : public Text::TextBinEnc::TextBinEnc
		{
		public:
			ASCII85Enc();
			virtual ~ASCII85Enc();
			virtual UOSInt EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UOSInt buffSize) const;
			UnsafeArray<UTF8Char> EncodeBin(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UInt8> dataBuff, UOSInt buffSize) const;
			virtual UOSInt CalcBinSize(Text::CStringNN a85Str) const;
			virtual UOSInt DecodeBin(Text::CStringNN a85Str, UnsafeArray<UInt8> dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
