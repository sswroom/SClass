#ifndef _SM_PARSER_COMPARSER
#define _SM_PARSER_COMPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class COMParser : public IO::FileParser
		{
		public:
			COMParser();
			virtual ~COMParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	};
};
#endif
