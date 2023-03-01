#ifndef _SM_PARSER_FILEPARSER_ICOPARSER
#define _SM_PARSER_FILEPARSER_ICOPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class ICOParser : public IO::FileParser
		{
		public:
			ICOParser();
			virtual ~ICOParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	};
};
#endif
