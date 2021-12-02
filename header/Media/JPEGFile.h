#ifndef _SM_MEDIA_JPEGFILE
#define _SM_MEDIA_JPEGFILE
#include "IO/IStreamData.h"
#include "Media/ICCProfile.h"
#include "Media/Image.h"
#include "Media/ImageList.h"
#include "Parser/ParserList.h"
#include "Text/XMLDOM.h"

namespace Media
{
	class JPEGFile
	{
	public:
		static Bool ParseJPEGHeader(IO::IStreamData *fd, Media::Image *img, Media::ImageList *imgList, Parser::ParserList *parsers);
		static Media::EXIFData *ParseJPEGExif(IO::IStreamData *fd);
		static Bool ParseJPEGHeaders(IO::IStreamData *fd, Media::EXIFData **exif, Text::XMLDocument **xmf, Media::ICCProfile **icc, UInt32 *width, UInt32 *height);
		static void WriteJPGBuffer(IO::Stream *stm, const UInt8 *jpgBuff, UOSInt buffSize, Media::Image *oriImg);
	};
}
#endif
