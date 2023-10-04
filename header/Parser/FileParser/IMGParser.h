#ifndef _SM_PARSER_FILEPARSER_IMGPARSER
#define _SM_PARSER_FILEPARSER_IMGPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class IMGParser : public IO::FileParser
		{
		public:
			IMGParser();
			virtual ~IMGParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
