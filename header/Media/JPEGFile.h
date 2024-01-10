#ifndef _SM_MEDIA_JPEGFILE
#define _SM_MEDIA_JPEGFILE
#include "IO/StreamData.h"
#include "Media/ICCProfile.h"
#include "Media/ImageList.h"
#include "Parser/ParserList.h"
#include "Text/XMLDOM.h"

namespace Media
{
	class JPEGFile
	{
	public:
		static Bool ParseJPEGHeader(NotNullPtr<IO::StreamData> fd, Media::RasterImage *img, Media::ImageList *imgList, Parser::ParserList *parsers);
		static Optional<Media::EXIFData> ParseJPEGExif(NotNullPtr<IO::StreamData> fd);
		static Bool ParseJPEGHeaders(NotNullPtr<IO::StreamData> fd, OutParam<Optional<Media::EXIFData>> exif, OutParam<Optional<Text::XMLDocument>> xmf, OutParam<Optional<Media::ICCProfile>> icc, OutParam<UInt32> width, OutParam<UInt32> height);
		static void WriteJPGBuffer(NotNullPtr<IO::Stream> stm, const UInt8 *jpgBuff, UOSInt buffSize, Media::RasterImage *oriImg);
	};
}
#endif
