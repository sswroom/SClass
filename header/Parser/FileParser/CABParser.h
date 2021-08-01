#ifndef _SM_PARSER_FILEPARSER_CABPARSER
#define _SM_PARSER_FILEPARSER_CABPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class CABParser : public IO::FileParser
		{
		public:
			CABParser();
			virtual ~CABParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
