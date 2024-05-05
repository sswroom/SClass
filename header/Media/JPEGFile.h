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
		static Bool ParseJPEGHeader(NN<IO::StreamData> fd, NN<Media::RasterImage> img, NN<Media::ImageList> imgList, Parser::ParserList *parsers);
		static Optional<Media::EXIFData> ParseJPEGExif(NN<IO::StreamData> fd);
		static Bool ParseJPEGHeaders(NN<IO::StreamData> fd, OutParam<Optional<Media::EXIFData>> exif, OutParam<Optional<Text::XMLDocument>> xmf, OutParam<Optional<Media::ICCProfile>> icc, OutParam<UInt32> width, OutParam<UInt32> height);
		static void WriteJPGBuffer(NN<IO::Stream> stm, const UInt8 *jpgBuff, UOSInt buffSize, Optional<Media::RasterImage> oriImg);
	};
}
#endif
