#ifndef _SM_PARSER_FILEPARSER_ISOPARSER
#define _SM_PARSER_FILEPARSER_ISOPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class ISOParser : public IO::FileParser
		{
		private:
			ParserList *parsers;

		public:
			ISOParser();
			virtual ~ISOParser();

			virtual Int32 GetName();
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
