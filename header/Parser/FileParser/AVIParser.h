#ifndef _SM_PARSER_FILEPARSER_AVIPARSER
#define _SM_PARSER_FILEPARSER_AVIPARSER
#include "IO/FileParser.h"
#include "Media/AudioFormat.h"

namespace Parser
{
	namespace FileParser
	{
		class AVIParser : public IO::FileParser
		{
		private:
			UInt32 codePage;
		public:
			AVIParser();
			virtual ~AVIParser();

			virtual Int32 GetName();
			virtual void SetCodePage(UInt32 codePage);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);

		private:
			UInt32 EstimateDecodeSize(NN<Media::AudioFormat> fmt, UInt64 totalSize, UIntOS frameSize);
		};
	}
}
#endif
