#ifndef _SM_TEXT_TEXTBINENC_ITEXTBINENC
#define _SM_TEXT_TEXTBINENC_ITEXTBINENC
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	namespace TextBinEnc
	{
		class ITextBinEnc
		{
		public:
			virtual ~ITextBinEnc(){};
			virtual UOSInt EncodeBin(NN<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize) const = 0;
			virtual UOSInt CalcBinSize(Text::CStringNN str) const = 0;
			virtual UOSInt DecodeBin(Text::CStringNN str, UInt8 *dataBuff) const = 0;
			virtual Text::CStringNN GetName() const = 0;
		};
	}
}
#endif
