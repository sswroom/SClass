#ifndef _SM_PARSER_FILEPARSER_TGAPARSER
#define _SM_PARSER_FILEPARSER_TGAPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class TGAParser : public IO::IFileParser
		{
		public:
			TGAParser();
			virtual ~TGAParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
