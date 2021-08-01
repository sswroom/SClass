#ifndef _SM_PARSER_FILEPARSER_MD5PARSER
#define _SM_PARSER_FILEPARSER_MD5PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class MD5Parser : public IO::FileParser
		{
		private:
			UInt32 codePage;
		public:
			MD5Parser();
			virtual ~MD5Parser();

			virtual Int32 GetName();
			virtual void SetCodePage(UInt32 codePage);
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
