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
		public:
			MDBParser();
			virtual ~MDBParser();

			virtual void SetCodePage(UInt32 codePage);

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
