#ifndef _SM_PARSER_FILEPARSER_XLSPARSER
#define _SM_PARSER_FILEPARSER_XLSPARSER
#include "Data/Int32Map.h"
#include "IO/FileParser.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/SpreadSheet/Workbook.h"

namespace Parser
{
	namespace FileParser
	{
		class XLSParser : public IO::FileParser
		{
		private:
			typedef struct
			{
				Text::SpreadSheet::Worksheet *ws;
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
				Text::String *fontName;
			} FontInfo;

			struct WorkbookStatus
			{
				Data::ArrayList<Text::String *> sst;
				Data::ArrayList<WorksheetStatus*> wsList;
				Data::ArrayList<FontInfo *> fontList;
				Data::Int32Map<Text::String *> formatMap;
				UInt32 palette[56];
			};

		public:
			XLSParser();
			virtual ~XLSParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		private:
			static Bool ParseWorkbook(IO::IStreamData *fd, UInt64 ofst, UInt64 ofstRef, Text::SpreadSheet::Workbook *wb);
			static Bool ParseWorksheet(IO::IStreamData *fd, UInt64 ofst, Text::SpreadSheet::Workbook *wb, Text::SpreadSheet::Worksheet *ws, WorkbookStatus *status);
			static UOSInt ReadUString(UInt8 *buff, Text::StringBuilderUTF8 *sb);
			static UOSInt ReadUStringPartial(UInt8 *buff, UOSInt buffSize, UInt32 *charCnt, Text::StringBuilderUTF8 *sb);
			static UOSInt ReadUStringB(UInt8 *buff, Text::StringBuilderUTF8 *sb);
			static Double ParseRKNumber(Int32 rkValue);
		};
	}
}
#endif
