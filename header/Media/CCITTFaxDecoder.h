#ifndef _SM_MEDIA_CCITTFAXDECODER
#define _SM_MEDIA_CCITTFAXDECODER
#include "Data/ByteArray.h"
#include "IO/FaxBitReader.h"
#include "Media/StaticImage.h"

namespace Media
{
	class CCITTFaxDecoder
	{
	private:
		static Bool ReadRun(IO::FaxBitReader &reader, Bool black, UIntOS width, UIntOS &run);
		static void FillRun(UnsafeArray<UInt8> row, UIntOS start, UIntOS end, Bool black);
		static Bool Decode1DLine(IO::FaxBitReader &reader, UnsafeArray<UInt8> row, UIntOS width);
		static Bool Read2DMode(IO::FaxBitReader &reader, Int32 &mode);
		static void GetTransitions(UnsafeArray<const UInt8> row, UIntOS width, Data::ArrayListNative<UIntOS> &transitions);
		static Bool Decode2DLine(IO::FaxBitReader &reader, UnsafeArray<UInt8> row, UnsafeArray<const UInt8> refRow, UIntOS width);
	public:
		CCITTFaxDecoder();
		~CCITTFaxDecoder();

		Optional<Media::StaticImage> DecodeImage(Data::ByteArrayR dataBuff, UIntOS width, UIntOS height, Int32 k = 0, Bool endOfLine = false, Bool encodedByteAlign = false) const;
	};
}
#endif
