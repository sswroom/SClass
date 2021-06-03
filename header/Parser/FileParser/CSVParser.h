#ifndef _SM_PARSER_FILEPARSER_CSVPARSER
#define _SM_PARSER_FILEPARSER_CSVPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class CSVParser : public IO::IFileParser
		{
		private:
			UInt32 codePage;
		public:
			CSVParser();
			virtual ~CSVParser();

			virtual Int32 GetName();
			virtual void SetCodePage(UInt32 codePage);
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
