#ifndef _SM_PARSER_FILEPARSER_CFBPARSER
#define _SM_PARSER_FILEPARSER_CFBPARSER
#include "Data/FastMap.h"
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
				NotNullPtr<Text::SpreadSheet::Worksheet> ws;
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
				NotNullPtr<Text::String> fontName;
			} FontInfo;

			struct WorkbookStatus
			{
				Data::ArrayListNN<Text::String> sst;
				Data::ArrayList<WorksheetStatus*> wsList;
				Data::ArrayList<FontInfo *> fontList;
				Data::FastMap<Int32, Text::String *> formatMap;
				UInt32 palette[56];
			};

		public:
			CFBParser();
			virtual ~CFBParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		private:
			static Bool ParseWorkbook(NotNullPtr<IO::StreamData> fd, UInt64 ofst, UInt64 ofstRef, NotNullPtr<Text::SpreadSheet::Workbook> wb);
			static Bool ParseWorksheet(NotNullPtr<IO::StreamData> fd, UInt64 ofst, NotNullPtr<Text::SpreadSheet::Workbook> wb, NotNullPtr<Text::SpreadSheet::Worksheet> ws, WorkbookStatus *status);
			static UOSInt ReadUString(UInt8 *buff, NotNullPtr<Text::StringBuilderUTF8> sb);
			static UOSInt ReadUStringPartial(UInt8 *buff, UOSInt buffSize, InOutParam<UInt32> charCnt, NotNullPtr<Text::StringBuilderUTF8> sb);
			static UOSInt ReadUStringB(UInt8 *buff, NotNullPtr<Text::StringBuilderUTF8> sb);
			static Double ParseRKNumber(Int32 rkValue);
		};
	}
}
#endif
