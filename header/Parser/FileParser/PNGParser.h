#ifndef _SM_PARSER_FILEPARSER_PNGPARSER
#define _SM_PARSER_FILEPARSER_PNGPARSER
#include "IO/IFileParser.h"
#include "Media/ImageList.h"

namespace Parser
{
	namespace FileParser
	{
		class PNGParser : public IO::IFileParser
		{
		private:
			static void ParseImage(UInt8 bitDepth, UInt8 colorType, UInt8 *dataBuff, Media::FrameInfo *info, Media::ImageList *imgList, Int32 imgDelay, Int32 imgX, Int32 imgY, UInt32 imgW, UInt32 imgH, UInt8 interlaceMeth, UInt8 *palette);
		public:
			PNGParser();
			virtual ~PNGParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);

			static UOSInt CalcImageSize(Int32 imgW, Int32 imgH, UInt8 bitDepth, UInt8 colorType, UInt8 interlaceMeth);
		};
	}
}
#endif
