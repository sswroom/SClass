#ifndef _SM_PARSER_FILEPARSER_MP2PARSER
#define _SM_PARSER_FILEPARSER_MP2PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class MP2Parser : public IO::FileParser
		{
		public:
			MP2Parser();
			virtual ~MP2Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
