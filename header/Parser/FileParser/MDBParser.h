#ifndef _SM_PARSER_FILEPARSER_MDBPARSER
#define _SM_PARSER_FILEPARSER_MDBPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class MDBParser : public IO::IFileParser
		{
		private:
			Int32 codePage;
		public:
			MDBParser();
			virtual ~MDBParser();

			virtual void SetCodePage(Int32 codePage);

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
