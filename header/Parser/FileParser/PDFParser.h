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
			struct PDFXRefItem
			{
				UInt64 ofst;
				UOSInt generation;
				UTF8Char type;
			};

			struct PDFXRef
			{
				UOSInt startId;
				UOSInt count;
				PDFXRefItem *items;
				PDFXRef *nextRef;
				Media::PDFParameter *trailer;
			};
		private:
			struct PDFParseEnv;

			Bool IsComment(const UTF8Char *buff, UOSInt size);
			Bool NextLine(PDFParseEnv *env, Text::StringBuilderUTF8 *sb, Bool skipComment);
			Bool NextLineFixed(PDFParseEnv *env, UOSInt size);
			void ParseStartxref(PDFParseEnv *env, Text::StringBuilderUTF8 *sb);
			Bool ParseObject(PDFParseEnv *env, Text::StringBuilderUTF8 *sb, Media::PDFDocument *doc, PDFXRef *xref);
			Bool ParseObjectStream(PDFParseEnv *env, Text::StringBuilderUTF8 *sb, Media::PDFObject *obj, PDFXRef *xref);
			PDFXRef *ParseXRef(PDFParseEnv *env, Text::StringBuilderUTF8 *sb);
			void FreeXRef(PDFXRef *xref);
		public:
			PDFParser();
			virtual ~PDFParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
