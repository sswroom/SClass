#ifndef _SM_MEDIA_JBIG2DECODER
#define _SM_MEDIA_JBIG2DECODER
#include "Data/ByteArray.h"
#include "Media/JBIG2ArithmeticDecoder.h"
#include "Media/StaticImage.h"

namespace Media
{
	class JBIG2Decoder
	{
	private:
		struct JBIG2Segment
		{
			UInt32 number;
			UInt8 type;
			UInt32 page;
			UnsafeArray<const UInt8> data;
			UIntOS dataSize;

			JBIG2Segment() = default;
			JBIG2Segment(std::nullptr_t)
			{
				number = 0;
				type = 0;
				page = 0;
				data = &type;
				dataSize = 0;
			}

			Bool operator==(const JBIG2Segment &other) const
			{
				return number == other.number && type == other.type && page == other.page && Text::StrEqualsC(data, dataSize, other.data, other.dataSize);
			}
		};

		struct JBIG2Glyph
		{
			UIntOS width;
			UIntOS height;
			UIntOS offset;

			JBIG2Glyph()
			{
				width = 0;
				height = 0;
				offset = 0;
			}

			JBIG2Glyph(Int32)
			{
				width = 0;
				height = 0;
				offset = 0;
			}

			Bool operator==(const JBIG2Glyph &other) const
			{
				return width == other.width && height == other.height && offset == other.offset;
			}
		};


	private:
		static Bool IsJBIG2File(UnsafeArray<const UInt8> data, UIntOS dataSize);
		static Bool ParseSegments(Data::ByteArrayR dataBuff, Data::ArrayListNative<JBIG2Segment> &segments);
		static Bool DecodeGenericBitmap(JBIG2ArithmeticDecoder &arith, UIntOS width, UIntOS height, const Int8 adaptive[8], Data::ByteBuffer &bitmap);
		static Bool DecodeSymbolDictionary(const JBIG2Segment &seg, Data::ByteBuffer &glyphData, Data::ArrayListNative<JBIG2Glyph> &glyphs);
		static void CombineGlyph(Data::ByteBuffer &region, UIntOS regionWidth, UIntOS regionHeight, const Data::ByteBuffer &glyphData, const JBIG2Glyph &glyph, IntOS x, IntOS y, UInt8 op);
		static Bool DecodeTextRegion(const JBIG2Segment &seg, Data::ByteBuffer &page, UIntOS pageWidth, UIntOS pageHeight, const Data::ByteBuffer &glyphData, const Data::ArrayListNative<JBIG2Glyph> &glyphs);
		static void CombineRegion(Data::ByteBuffer &page, UIntOS pageWidth, UIntOS pageHeight, const Data::ByteBuffer &region, UIntOS regionWidth, UIntOS regionHeight, Int32 regionX, Int32 regionY, UInt8 op);
		static Optional<Media::StaticImage> DecodeJBIG2Image(Data::ByteArrayR dataBuff, UIntOS width, UIntOS height, Data::ByteArrayR globalData);
	public:
		JBIG2Decoder();
		~JBIG2Decoder();

		Optional<Media::StaticImage> DecodeImage(Data::ByteArrayR dataBuff, UIntOS width = 0, UIntOS height = 0) const;
		Optional<Media::StaticImage> DecodeImage(Data::ByteArrayR dataBuff, UIntOS width, UIntOS height, Data::ByteArrayR globalData) const;
	};
}
#endif
