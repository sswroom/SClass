#ifndef _SM_TEXT_TEXTBINENC_TEXTBINENC
#define _SM_TEXT_TEXTBINENC_TEXTBINENC
#include "Data/ByteArray.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	namespace TextBinEnc
	{
		class TextBinEnc
		{
		public:
			virtual ~TextBinEnc(){};
			virtual UIntOS EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UIntOS buffSize) const = 0;
			UIntOS EncodeBArr(NN<Text::StringBuilderUTF8> sb, Data::ByteArrayR dataBuff) const
			{
				return this->EncodeBin(sb, dataBuff.Arr(), dataBuff.GetSize());
			}
			virtual UIntOS CalcBinSize(Text::CStringNN str) const = 0;
			virtual UIntOS DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const = 0;
			virtual Text::CStringNN GetName() const = 0;
		};
	}
}
#endif
