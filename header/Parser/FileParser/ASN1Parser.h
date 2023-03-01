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
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
