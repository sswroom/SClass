#ifndef _SM_PARSER_FILEPARSER_SEGPACKPARSER
#define _SM_PARSER_FILEPARSER_SEGPACKPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SEGPackParser : public IO::IFileParser
		{
		public:
			SEGPackParser();
			virtual ~SEGPackParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
