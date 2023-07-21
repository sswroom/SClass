#ifndef _SM_PARSER_FILEPARSER_CSVPARSER
#define _SM_PARSER_FILEPARSER_CSVPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class CSVParser : public IO::FileParser
		{
		private:
			UInt32 codePage;
		public:
			CSVParser();
			virtual ~CSVParser();

			virtual Int32 GetName();
			virtual void SetCodePage(UInt32 codePage);
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
