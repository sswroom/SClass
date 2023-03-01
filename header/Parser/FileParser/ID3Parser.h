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
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);

			static UInt32 ReadUSInt32(const UInt8 *buff);
		};
	}
}
#endif
