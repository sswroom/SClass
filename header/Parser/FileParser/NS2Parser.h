#ifndef _SM_PARSER_FILEPARSER_NS2PARSER
#define _SM_PARSER_FILEPARSER_NS2PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class NS2Parser : public IO::FileParser
		{
		public:
			NS2Parser();
			virtual ~NS2Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
