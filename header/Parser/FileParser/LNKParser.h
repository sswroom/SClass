#ifndef _SM_PARSER_FILEPARSER_LNKPARSER
#define _SM_PARSER_FILEPARSER_LNKPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class LNKParser : public IO::FileParser
		{
		public:
			LNKParser();
			virtual ~LNKParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
