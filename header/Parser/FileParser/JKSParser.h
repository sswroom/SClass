#ifndef _SM_PARSER_FILEPARSER_JKSPARSER
#define _SM_PARSER_FILEPARSER_JKSPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class JKSParser : public IO::FileParser
		{
		public:
			JKSParser();
			virtual ~JKSParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
