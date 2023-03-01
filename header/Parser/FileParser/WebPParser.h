#ifndef _SM_PARSER_FILEPARSER_WEBPPARSER
#define _SM_PARSER_FILEPARSER_WEBPPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class WebPParser : public IO::FileParser
		{
		public:
			WebPParser();
			virtual ~WebPParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
