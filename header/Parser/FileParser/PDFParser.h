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
				UIntOS generation;
				UTF8Char type;
			};

			struct PDFXRef
			{
				UIntOS startId;
				UIntOS count;
				PDFXRefItem *items;
				Optional<PDFXRef> nextRef;
				Optional<Media::PDFParameter> trailer;
			};
		private:
			struct PDFParseEnv;

			static UInt8 HexValue(UTF8Char c);
			static Bool ParseHex(Text::CStringNN value, UnsafeArray<UInt8> output, UIntOS outputSize);
			static Bool ParsePDFString(Text::CStringNN value, UnsafeArray<UInt8> output, UIntOS outputSize);
			static Bool DeriveStandardRC4Key(NN<Media::PDFParameter> encryption, Text::CStringNN fileID, UnsafeArray<UInt8> fileKey, UIntOS &keyLen);

			void SetupEncryption(NN<Media::PDFDocument> doc, NN<PDFXRef> xref);
			Bool IsComment(UnsafeArray<const UTF8Char> buff, UIntOS size);
			Bool NextLine(NN<PDFParseEnv> env, NN<Text::StringBuilderUTF8> sb, Bool skipComment);
			Bool NextLineFixed(NN<PDFParseEnv> env, UIntOS size);
			void ParseStartxref(NN<PDFParseEnv> env, NN<Text::StringBuilderUTF8> sb);
			Bool ParseObject(NN<PDFParseEnv> env, NN<Text::StringBuilderUTF8> sb, NN<Media::PDFDocument> doc, Optional<PDFXRef> xref);
			Bool ParseObjectStream(NN<PDFParseEnv> env, NN<Text::StringBuilderUTF8> sb, NN<Media::PDFObject> obj, Optional<PDFXRef> xref);
			Optional<PDFXRef> ParseXRef(NN<PDFParseEnv> env, NN<Text::StringBuilderUTF8> sb);
			void FreeXRef(NN<PDFXRef> xref);
		public:
			PDFParser();
			virtual ~PDFParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
