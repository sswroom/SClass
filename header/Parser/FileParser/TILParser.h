#ifndef _SM_PARSER_FILEPARSER_TILPARSER
#define _SM_PARSER_FILEPARSER_TILPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class TILParser : public IO::IFileParser
		{
		public:
			TILParser();
			virtual ~TILParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
