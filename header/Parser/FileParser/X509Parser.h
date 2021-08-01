#ifndef _SM_PARSER_FILEPARSER_X509PARSER
#define _SM_PARSER_FILEPARSER_X509PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class X509Parser : public IO::FileParser
		{
		public:
			X509Parser();
			virtual ~X509Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	}
}
#endif
