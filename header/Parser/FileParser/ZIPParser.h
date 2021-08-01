#ifndef _SM_PARSER_FILEPARSER_ZIPPARSER
#define _SM_PARSER_FILEPARSER_ZIPPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class ZIPParser : public IO::FileParser
		{
		private:
			UInt32 codePage;
			Text::EncodingFactory *encFact;
			Parser::ParserList *parsers;
		public:
			ZIPParser();
			virtual ~ZIPParser();

			virtual Int32 GetName();
			virtual void SetCodePage(UInt32 codePage);
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void SetEncFactory(Text::EncodingFactory *encFact);
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
