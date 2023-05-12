#ifndef _SM_PARSER_FILEPARSER_PDFPARSER
#define _SM_PARSER_FILEPARSER_PDFPARSER
#include "IO/FileParser.h"
#include "Media/PDFDocument.h"
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
			void ParseStartxref(PDFParseEnv *env, Text::StringBuilderUTF8 *sb);
			Bool ParseObject(PDFParseEnv *env, Text::StringBuilderUTF8 *sb, Media::PDFDocument *doc);
			Bool ParseObjectStream(PDFParseEnv *env, Text::StringBuilderUTF8 *sb, Media::PDFObject *obj);
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
