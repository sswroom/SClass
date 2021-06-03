#ifndef _SM_PARSER_FILEPARSER_TARPARSER
#define _SM_PARSER_FILEPARSER_TARPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class TARParser : public IO::IFileParser
		{
		private:
			Int32 codePage;
		public:
			TARParser();
			virtual ~TARParser();

			virtual Int32 GetName();
			virtual void SetCodePage(UInt32 codePage);
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
