#ifndef _SM_PARSER_FILEPARSER_JXLPARSER
#define _SM_PARSER_FILEPARSER_JXLPARSER
#include "IO/FileParser.h"
#include "Media/StaticImage.h"

namespace Parser
{
	namespace FileParser
	{
		class JXLParser : public IO::FileParser
		{
		private:
			struct BoxData
			{
				Optional<Media::EXIFData> exif;
			};
		public:
			JXLParser();
			virtual ~JXLParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		private:
			static void FreeBoxData(NN<BoxData> boxData);
			void ParseBox(NN<BoxData> boxData, Data::ByteArrayR boxArr, UInt32 boxType);
		};
	}
}
#endif
