#ifndef _SM_PARSER_FILEPARSER_XPCMPARSER
#define _SM_PARSER_FILEPARSER_XPCMPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class XPCMParser : public IO::FileParser
		{
		public:
			XPCMParser();
			virtual ~XPCMParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
