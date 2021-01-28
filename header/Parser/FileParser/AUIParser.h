#ifndef _SM_PARSER_FILEPARSER_AUIPARSER
#define _SM_PARSER_FILEPARSER_AUIPARSER
#include "IO/IFileParser.h"
#include "Media/AVIUtl/AUIManager.h"

namespace Parser
{
	namespace FileParser
	{
		class AUIParser : public IO::IFileParser
		{
		private:
			Media::AVIUtl::AUIManager *auiMgr;
		public:
			AUIParser();
			virtual ~AUIParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
