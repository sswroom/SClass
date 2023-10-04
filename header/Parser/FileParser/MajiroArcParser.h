#ifndef _SM_PARSER_FILEPARSER_MAJIROARCPARSER
#define _SM_PARSER_FILEPARSER_MAJIROARCPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class MajiroArcParser : public IO::FileParser
		{
		public:
			MajiroArcParser();
			virtual ~MajiroArcParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
