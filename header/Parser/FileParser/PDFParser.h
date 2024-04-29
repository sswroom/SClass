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
			Bool NextLine(NN<PDFParseEnv> env, NN<Text::StringBuilderUTF8> sb, Bool skipComment);
			Bool NextLineFixed(NN<PDFParseEnv> env, UOSInt size);
			void ParseStartxref(NN<PDFParseEnv> env, NN<Text::StringBuilderUTF8> sb);
			Bool ParseObject(NN<PDFParseEnv> env, NN<Text::StringBuilderUTF8> sb, NN<Media::PDFDocument> doc, PDFXRef *xref);
			Bool ParseObjectStream(NN<PDFParseEnv> env, NN<Text::StringBuilderUTF8> sb, NN<Media::PDFObject> obj, PDFXRef *xref);
			PDFXRef *ParseXRef(NN<PDFParseEnv> env, NN<Text::StringBuilderUTF8> sb);
			void FreeXRef(PDFXRef *xref);
		public:
			PDFParser();
			virtual ~PDFParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
