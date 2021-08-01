#ifndef _SM_PARSER_FILEPARSER_IMGPARSER
#define _SM_PARSER_FILEPARSER_IMGPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class IMGParser : public IO::FileParser
		{
		public:
			IMGParser();
			virtual ~IMGParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
