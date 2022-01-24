#ifndef _SM_TEXT_TEXTBINENC_ITEXTBINENC
#define _SM_TEXT_TEXTBINENC_ITEXTBINENC
#include "Text/CString.h"
#include "Text/StringBuilderUTF.h"

namespace Text
{
	namespace TextBinEnc
	{
		class ITextBinEnc
		{
		public:
			virtual ~ITextBinEnc(){};
			virtual UOSInt EncodeBin(Text::StringBuilderUTF *sb, const UInt8 *dataBuff, UOSInt buffSize) = 0;
			virtual UOSInt CalcBinSize(const UTF8Char *str, UOSInt strLen) = 0;
			virtual UOSInt DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff) = 0;
			virtual Text::CString GetName() = 0;
		};
	}
}
#endif
