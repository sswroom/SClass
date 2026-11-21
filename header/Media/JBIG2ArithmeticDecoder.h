#ifndef _SM_MEDIA_JBIG2ARITHMETICDECODER
#define _SM_MEDIA_JBIG2ARITHMETICDECODER
#include "Data/ByteBuffer.h"

namespace Media
{
	class JBIG2ArithmeticDecoder
	{
	private:
		struct JBIG2Qe
		{
			UInt16 qe;
			UInt8 nmps;
			UInt8 nlps;
			Bool switchMPS;
		};

		static const JBIG2Qe jbig2Qe[];

	private:
		UnsafeArray<const UInt8> data;
		UIntOS dataSize;
		UIntOS pos;
		UInt32 a;
		UInt32 c;
		Int32 ct;
		UInt8 previousByte;
		Data::ByteBuffer contexts;

		Bool ByteIn();

	public:
		JBIG2ArithmeticDecoder(UnsafeArray<const UInt8> data, UIntOS dataSize);

		Int32 Decode(UIntOS cx);
		Bool DecodeInteger(UIntOS contextBase, Int32 &value, Bool &outOfBounds);
		Bool DecodeSymbolId(UIntOS codeLength, UIntOS &id);
	};
}
#endif