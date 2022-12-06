#ifndef _SM_PARSER_FILEPARSER_PFS2PARSER
#define _SM_PARSER_FILEPARSER_PFS2PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class PFS2Parser : public IO::FileParser
		{
		public:
			PFS2Parser();
			virtual ~PFS2Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
