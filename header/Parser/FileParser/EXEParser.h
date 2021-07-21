#ifndef _SM_PARSER_FILEPARSER_EXEPARSER
#define _SM_PARSER_FILEPARSER_EXEPARSER
#include "IO/EXEFile.h"
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class EXEParser : public IO::IFileParser
		{
		public:
			EXEParser();
			virtual ~EXEParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);

			void ParseResource(IO::EXEFile *exef, UInt32 resType, UTF8Char *sbuff, UTF8Char *sbuffEnd, UInt8 *resBuff, UOSInt resOfst, UInt8 *exeImage);
			void ParseResourceData(IO::EXEFile *exef, UInt32 resType, UTF8Char *sbuff, UTF8Char *sbuffEnd, UInt8 *resBuff, UOSInt resOfst, UInt8 *exeImage);
			IO::EXEFile::ResourceType GetResourceType(UInt32 resType);
		};
	}
}
#endif
