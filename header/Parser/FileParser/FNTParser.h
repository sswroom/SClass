#ifndef _SM_PARSER_FILEPARSER_FNTPARSER
#define _SM_PARSER_FILEPARSER_FNTPARSER
#include "IO/FileParser.h"
#include "Media/FontRenderer.h"
#include "Text/StringBuilderUTF8.h"

namespace Parser
{
	namespace FileParser
	{
		class FNTParser : public IO::FileParser
		{
		public:
			FNTParser();
			virtual ~FNTParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType);

			static Media::FontRenderer *ParseFontBuff(NotNullPtr<Text::String> sourceName, const UInt8 *fontBuff, UOSInt buffSize);
			static UOSInt GetFileDesc(const UInt8 *fileBuff, UOSInt fileSize, NotNullPtr<Text::StringBuilderUTF8> sb); //return header size
			static void GetFileDirDesc(const UInt8 *fileBuff, UOSInt fileSize, NotNullPtr<Text::StringBuilderUTF8> sb);
		};
	}
}
#endif
