#ifndef _SM_TEXT_TEXTBINENC_CPPBYTEARRBINENC
#define _SM_TEXT_TEXTBINENC_CPPBYTEARRBINENC
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class CPPByteArrBinEnc : public Text::TextBinEnc::ITextBinEnc
		{
		public:
			CPPByteArrBinEnc();
			virtual ~CPPByteArrBinEnc();
			virtual UOSInt EncodeBin(NN<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize) const;
			virtual UOSInt CalcBinSize(const UTF8Char *str, UOSInt strLen) const;
			virtual UOSInt DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
