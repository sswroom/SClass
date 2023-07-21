#ifndef _SM_PARSER_FILEPARSER_EXEPARSER
#define _SM_PARSER_FILEPARSER_EXEPARSER
#include "IO/EXEFile.h"
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class EXEParser : public IO::FileParser
		{
		public:
			EXEParser();
			virtual ~EXEParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);

			void ParseResource(IO::EXEFile *exef, UInt32 resType, UTF8Char *sbuff, UTF8Char *sbuffEnd, UInt8 *resBuff, UOSInt resOfst, UInt8 *exeImage);
			void ParseResourceData(IO::EXEFile *exef, UInt32 resType, UTF8Char *sbuff, UTF8Char *sbuffEnd, UInt8 *resBuff, UOSInt resOfst, UInt8 *exeImage);
			IO::EXEFile::ResourceType GetResourceType(UInt32 resType);
		};
	}
}
#endif
