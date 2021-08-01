#ifndef _SM_PARSER_FILEPARSER_MRGPARSER
#define _SM_PARSER_FILEPARSER_MRGPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class MRGParser : public IO::FileParser
		{
		public:
			MRGParser();
			virtual ~MRGParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
