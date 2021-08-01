#ifndef _SM_PARSER_FILEPARSER_DSHOWPARSER
#define _SM_PARSER_FILEPARSER_DSHOWPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class DShowParser : public IO::FileParser
		{
		public:
			DShowParser();
			virtual ~DShowParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
