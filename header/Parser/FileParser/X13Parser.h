#ifndef _SM_PARSER_FILEPARSER_X13PARSER
#define _SM_PARSER_FILEPARSER_X13PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class X13Parser : public IO::FileParser
		{
		public:
			X13Parser();
			virtual ~X13Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
