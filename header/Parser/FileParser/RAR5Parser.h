#ifndef _SM_PARSER_FILEPARSER_RAR5PARSER
#define _SM_PARSER_FILEPARSER_RAR5PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class RAR5Parser : public IO::FileParser
		{
		private:
			static const UInt8 *ReadVInt(const UInt8 *buffPtr, UInt64 *val);
		public:
			RAR5Parser();
			virtual ~RAR5Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
