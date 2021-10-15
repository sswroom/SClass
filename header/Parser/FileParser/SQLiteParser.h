#ifndef _SM_PARSER_FILEPARSER_SQLITEPARSER
#define _SM_PARSER_FILEPARSER_SQLITEPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SQLiteParser : public IO::FileParser
		{
		public:
			SQLiteParser();
			virtual ~SQLiteParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
