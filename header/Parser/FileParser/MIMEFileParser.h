#ifndef _SM_PARSER_FILEPARSER_MIMEFILEPARSER
#define _SM_PARSER_FILEPARSER_MIMEFILEPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class MIMEFileParser : public IO::FileParser
		{
		public:
			MIMEFileParser();
			virtual ~MIMEFileParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
