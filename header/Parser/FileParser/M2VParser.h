#ifndef _SM_PARSER_FILEPARSER_M2VPARSER
#define _SM_PARSER_FILEPARSER_M2VPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class M2VParser : public IO::IFileParser
		{
		public:
			M2VParser();
			virtual ~M2VParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
