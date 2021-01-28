#ifndef _SM_PARSER_FILEPARSER_NFPPARSER
#define _SM_PARSER_FILEPARSER_NFPPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class NFPParser : public IO::IFileParser
		{
		public:
			NFPParser();
			virtual ~NFPParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
