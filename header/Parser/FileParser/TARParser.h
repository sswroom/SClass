#ifndef _SM_PARSER_FILEPARSER_TARPARSER
#define _SM_PARSER_FILEPARSER_TARPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class TARParser : public IO::FileParser
		{
		private:
			UInt32 codePage;
		public:
			TARParser();
			virtual ~TARParser();

			virtual Int32 GetName();
			virtual void SetCodePage(UInt32 codePage);
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
