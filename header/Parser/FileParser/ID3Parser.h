#ifndef _SM_PARSER_FILEPARSER_ID3PARSER
#define _SM_PARSER_FILEPARSER_ID3PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class ID3Parser : public IO::FileParser
		{
		private:
			UInt32 codePage;
		public:
			ID3Parser();
			virtual ~ID3Parser();

			virtual Int32 GetName();
			virtual void SetCodePage(UInt32 codePage);
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);

			static UInt32 ReadUSInt32(UInt8 *buff);
		};
	};
};
#endif
