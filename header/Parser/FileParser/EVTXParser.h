#ifndef _SM_PARSER_FILEPARSER_EVTXPARSER
#define _SM_PARSER_FILEPARSER_EVTXPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class EVTXParser : public IO::FileParser
		{
		public:
			EVTXParser();
			virtual ~EVTXParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);

			static UOSInt ParseBinXML(UnsafeArray<const UInt8> chunk, UOSInt ofst, UOSInt endOfst, NN<Text::StringBuilderUTF8> sb, const UTF16Char *elementName, UOSInt nNameChar);
		};
	}
}
#endif
