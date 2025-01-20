#ifndef _SM_TEXT_TEXTBINENC_CPPBYTEARRBINENC
#define _SM_TEXT_TEXTBINENC_CPPBYTEARRBINENC
#include "Text/TextBinEnc/TextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class CPPByteArrBinEnc : public Text::TextBinEnc::TextBinEnc
		{
		public:
			CPPByteArrBinEnc();
			virtual ~CPPByteArrBinEnc();
			virtual UOSInt EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UOSInt buffSize) const;
			virtual UOSInt CalcBinSize(Text::CStringNN str) const;
			virtual UOSInt DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const;
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
