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
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);

		private:
			UInt32 EstimateDecodeSize(Media::AudioFormat *fmt, UInt64 totalSize, UOSInt frameSize);
		};
	}
}
#endif
