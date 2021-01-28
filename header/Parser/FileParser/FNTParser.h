#ifndef _SM_PARSER_FILEPARSER_FNTPARSER
#define _SM_PARSER_FILEPARSER_FNTPARSER
#include "IO/IFileParser.h"
#include "Media/FontRenderer.h"
#include "Text/StringBuilderUTF.h"

namespace Parser
{
	namespace FileParser
	{
		class FNTParser : public IO::IFileParser
		{
		public:
			FNTParser();
			virtual ~FNTParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);

			static Media::FontRenderer *ParseFontBuff(const UTF8Char *sourceName, const UInt8 *fontBuff, OSInt buffSize);
			static OSInt GetFileDesc(const UInt8 *fileBuff, OSInt fileSize, Text::StringBuilderUTF *sb); //return header size
			static void GetFileDirDesc(const UInt8 *fileBuff, OSInt fileSize, Text::StringBuilderUTF *sb);
		};
	};
};
#endif
