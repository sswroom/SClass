#ifndef _SM_PARSER_FILEPARSER_NS2PARSER
#define _SM_PARSER_FILEPARSER_NS2PARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class NS2Parser : public IO::IFileParser
		{
		public:
			NS2Parser();
			virtual ~NS2Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
