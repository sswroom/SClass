#ifndef _SM_PARSER_FILEPARSER_JP2PARSER
#define _SM_PARSER_FILEPARSER_JP2PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class JP2Parser : public IO::FileParser
		{
		public:
			JP2Parser();
			virtual ~JP2Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
