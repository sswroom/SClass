#ifndef _SM_PARSER_FILEPARSER_SHPPARSER
#define _SM_PARSER_FILEPARSER_SHPPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SHPParser : public IO::FileParser
		{
		private:
			UInt32 codePage;
		public:
			SHPParser();
			virtual ~SHPParser();

			virtual Int32 GetName();
			virtual void SetCodePage(UInt32 codePage);
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
