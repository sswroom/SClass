#ifndef _SM_PARSER_FILEPARSER_SHPPARSER
#define _SM_PARSER_FILEPARSER_SHPPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SHPParser : public IO::IFileParser
		{
		private:
			Int32 codePage;
		public:
			SHPParser();
			virtual ~SHPParser();

			virtual Int32 GetName();
			virtual void SetCodePage(Int32 codePage);
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
