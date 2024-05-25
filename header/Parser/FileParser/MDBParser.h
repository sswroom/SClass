#ifndef _SM_PARSER_FILEPARSER_MDBPARSER
#define _SM_PARSER_FILEPARSER_MDBPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class MDBParser : public IO::FileParser
		{
		private:
			UInt32 codePage;
			Optional<Math::ArcGISPRJParser> prjParser;
			Optional<IO::LogTool> log;
		public:
			MDBParser();
			virtual ~MDBParser();

			virtual void SetCodePage(UInt32 codePage);

			virtual Int32 GetName();
			virtual void SetArcGISPRJParser(Optional<Math::ArcGISPRJParser> prjParser);
			virtual void SetLogTool(Optional<IO::LogTool> log);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
