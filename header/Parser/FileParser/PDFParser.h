#ifndef _SM_PARSER_FILEPARSER_PDFPARSER
#define _SM_PARSER_FILEPARSER_PDFPARSER
#include "IO/FileParser.h"
#include "Text/StringBuilderUTF8.h"

namespace Parser
{
	namespace FileParser
	{
		class PDFParser : public IO::FileParser
		{
		private:
			struct PDFParseEnv;

			Bool IsComment(const UTF8Char *buff, UOSInt size);
			Bool NextLine(PDFParseEnv *env, Text::StringBuilderUTF8 *sb, Bool skipComment);
			Bool NextLineFixed(PDFParseEnv *env, UOSInt size);
		public:
			PDFParser();
			virtual ~PDFParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
