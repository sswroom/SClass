#ifndef _SM_PARSER_FILEPARSER_CABPARSER
#define _SM_PARSER_FILEPARSER_CABPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class CABParser : public IO::FileParser
		{
		public:
			CABParser();
			virtual ~CABParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
