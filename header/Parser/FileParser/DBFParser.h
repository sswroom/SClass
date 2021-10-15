#ifndef _SM_PARSER_FILEPARSER_DBFPARSER
#define _SM_PARSER_FILEPARSER_DBFPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class DBFParser : public IO::FileParser
		{
		private:
			UInt32 codePage;
		public:
			DBFParser();
			virtual ~DBFParser();

			virtual Int32 GetName();
			virtual void SetCodePage(UInt32 codePage);
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
