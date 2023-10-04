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
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	};
};
#endif
