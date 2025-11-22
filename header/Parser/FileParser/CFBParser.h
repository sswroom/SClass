#ifndef _SM_PARSER_FILEPARSER_CFBPARSER
#define _SM_PARSER_FILEPARSER_CFBPARSER
#include "Data/ArrayListStringNN.h"
#include "Data/FastMap.hpp"
#include "IO/FileParser.h"
#include "Text/SpreadSheet/Workbook.h"

namespace Parser
{
	namespace FileParser
	{
		class CFBParser : public IO::FileParser
		{
		private:
			typedef struct
			{
				NN<Text::SpreadSheet::Worksheet> ws;
				UInt64 ofst;
			} WorksheetStatus;

			typedef struct
			{
				Int32 height;
				Int32 grbit;
				Int32 icv;
				Int32 bls;
				Int32 sss;
				Int32 uls;
				Int32 bFamily;
				Int32 bCharSet;
				NN<Text::String> fontName;
			} FontInfo;

			struct WorkbookStatus
			{
				Data::ArrayListStringNN sst;
				Data::ArrayList<WorksheetStatus*> wsList;
				Data::ArrayList<FontInfo *> fontList;
				Data::FastMap<Int32, Text::String *> formatMap;
				UInt32 palette[56];
			};

		public:
			CFBParser();
			virtual ~CFBParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		private:
			static Bool ParseWorkbook(NN<IO::StreamData> fd, UInt64 ofst, UInt64 ofstRef, NN<Text::SpreadSheet::Workbook> wb);
			static Bool ParseWorksheet(NN<IO::StreamData> fd, UInt64 ofst, NN<Text::SpreadSheet::Workbook> wb, NN<Text::SpreadSheet::Worksheet> ws, WorkbookStatus *status);
			static UOSInt ReadUString(UInt8 *buff, NN<Text::StringBuilderUTF8> sb);
			static UOSInt ReadUStringPartial(UInt8 *buff, UOSInt buffSize, InOutParam<UInt32> charCnt, NN<Text::StringBuilderUTF8> sb);
			static UOSInt ReadUStringB(UInt8 *buff, NN<Text::StringBuilderUTF8> sb);
			static Double ParseRKNumber(Int32 rkValue);
		};
	}
}
#endif
