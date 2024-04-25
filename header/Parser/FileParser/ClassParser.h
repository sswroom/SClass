#ifndef _SM_PARSER_FILEPARSER_CLASSPARSER
#define _SM_PARSER_FILEPARSER_CLASSPARSER
#include "IO/FileParser.h"
#include "IO/JavaClass.h"

namespace Parser
{
	namespace FileParser
	{
		class ClassParser : public IO::FileParser
		{
		public:
			ClassParser();
			virtual ~ClassParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
