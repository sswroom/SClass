#ifndef _SM_PARSER_FILEPARSER_GAMEDATPAC2PARSER
#define _SM_PARSER_FILEPARSER_GAMEDATPAC2PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class GamedatPac2Parser : public IO::FileParser
		{
		public:
			GamedatPac2Parser();
			virtual ~GamedatPac2Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
