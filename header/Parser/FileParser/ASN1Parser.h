#ifndef _SM_PARSER_FILEPARSER_ASN1PARSER
#define _SM_PARSER_FILEPARSER_ASN1PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class ASN1Parser : public IO::FileParser
		{
		public:
			ASN1Parser();
			virtual ~ASN1Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	}
}
#endif
