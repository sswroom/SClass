#ifndef _SM_PARSER_FILEPARSER_ZIPPARSER
#define _SM_PARSER_FILEPARSER_ZIPPARSER
#include "IO/FileParser.h"
#include "IO/PackageFile.h"

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
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);

		private:
			static UOSInt ParseCentDir(IO::PackageFile *pf, Text::Encoding *enc, IO::IStreamData *fd, const UInt8 *buff, UOSInt buffSize, UInt64 ofst);
		};
	}
}
#endif
