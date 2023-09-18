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
			Math::ArcGISPRJParser *prjParser;
			IO::LogTool *log;
		public:
			MDBParser();
			virtual ~MDBParser();

			virtual void SetCodePage(UInt32 codePage);

			virtual Int32 GetName();
			virtual void SetArcGISPRJParser(Math::ArcGISPRJParser *prjParser);
			virtual void SetLogTool(IO::LogTool *log);
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
