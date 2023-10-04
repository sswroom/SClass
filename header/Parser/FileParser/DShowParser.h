#ifndef _SM_PARSER_FILEPARSER_DSHOWPARSER
#define _SM_PARSER_FILEPARSER_DSHOWPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class DShowParser : public IO::FileParser
		{
		public:
			DShowParser();
			virtual ~DShowParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
