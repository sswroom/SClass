#ifndef _SM_PARSER_FILEPARSER_PNGPARSER
#define _SM_PARSER_FILEPARSER_PNGPARSER
#include "IO/FileParser.h"
#include "Media/ImageList.h"

namespace Parser
{
	namespace FileParser
	{
		class PNGParser : public IO::FileParser
		{
		private:
			static void ParseImage(UInt8 bitDepth, UInt8 colorType, UnsafeArray<UInt8> dataBuff, NN<Media::FrameInfo> info, Media::ImageList *imgList, UInt32 imgDelay, UInt32 imgX, UInt32 imgY, UInt32 imgW, UInt32 imgH, UInt8 interlaceMeth, UInt8 *palette);
		public:
			PNGParser();
			virtual ~PNGParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);

			static UOSInt CalcImageSize(UInt32 imgW, UInt32 imgH, UInt8 bitDepth, UInt8 colorType, UInt8 interlaceMeth);
		};
	}
}
#endif
