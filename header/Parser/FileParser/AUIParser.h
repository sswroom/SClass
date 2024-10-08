#ifndef _SM_PARSER_FILEPARSER_AUIPARSER
#define _SM_PARSER_FILEPARSER_AUIPARSER
#include "IO/FileParser.h"
#include "Media/AVIUtl/AUIManager.h"

namespace Parser
{
	namespace FileParser
	{
		class AUIParser : public IO::FileParser
		{
		private:
			NN<Media::AVIUtl::AUIManager> auiMgr;
		public:
			AUIParser();
			virtual ~AUIParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
