#ifndef _SM_TEXT_SPREADSHEET_WORKBOOK
#define _SM_TEXT_SPREADSHEET_WORKBOOK
#include "IO/ParsedObject.h"
#include "Data/DateTime.h"
#include "Text/SpreadSheet/IStyleCtrl.h"
#include "Text/SpreadSheet/WorkbookFont.h"
#include "Text/SpreadSheet/Worksheet.h"

namespace Text
{
	namespace SpreadSheet
	{
		class Workbook : public IO::ParsedObject, public Text::SpreadSheet::IStyleCtrl
		{
		private:
			const UTF8Char *author;
			const UTF8Char *lastAuthor;
			const UTF8Char *company;
			Data::DateTime *createTime;
			Data::DateTime *modifyTime;
			Double version;
			Int32 windowTopX;
			Int32 windowTopY;
			Int32 windowWidth;
			Int32 windowHeight;
			UOSInt activeSheet;
			UInt32 palette[56];

			Data::ArrayList<Worksheet*> *sheets;
			Data::ArrayList<CellStyle*> *styles;
			Data::ArrayList<WorkbookFont*> *fonts;

			static const UInt32 defPalette[56];
		public:
			Workbook();
			virtual ~Workbook();

			virtual IO::ParserType GetParserType();

			Workbook *Clone();

			void AddDefaultStyles();

			void SetAuthor(const UTF8Char *author);
			void SetLastAuthor(const UTF8Char *lastAuthor);
			void SetCompany(const UTF8Char *company);
			void SetCreateTime(Data::DateTime *createTime);
			void SetModifyTime(Data::DateTime *modifyTime);
			void SetVersion(Double version);
			const UTF8Char *GetAuthor();
			const UTF8Char *GetLastAuthor();
			const UTF8Char *GetCompany();
			Data::DateTime *GetCreateTime();
			Data::DateTime *GetModifyTime();
			Double GetVersion();
			Bool HasInfo();

			void SetWindowTopX(Int32 windowTopX);
			void SetWindowTopY(Int32 windowTopY);
			void SetWindowWidth(Int32 windowWidth);
			void SetWindowHeight(Int32 windowHeight);
			void SetActiveSheet(UOSInt index);
			Int32 GetWindowTopX();
			Int32 GetWindowTopY();
			Int32 GetWindowWidth();
			Int32 GetWindowHeight();
			UOSInt GetActiveSheet();
			Bool HasWindowInfo();

			Bool HasCellStyle();
			CellStyle *NewCellStyle();
			CellStyle *NewCellStyle(WorkbookFont *font, HAlignment halign, VAlignment valign, const UTF8Char *dataFormat);
			UOSInt GetStyleCount();
			virtual OSInt GetStyleIndex(CellStyle *style);
			virtual CellStyle *GetStyle(UOSInt Index);
			CellStyle *GetDefaultStyle();
			void GetPalette(UInt32 *palette);
			void SetPalette(UInt32 *palette);

			Worksheet *AddWorksheet();
			Worksheet *AddWorksheet(const UTF8Char *name);
			Worksheet *InsertWorksheet(UOSInt index, const UTF8Char *name);
			UOSInt GetCount();
			Worksheet *GetItem(UOSInt index);
			void RemoveAt(UOSInt index);

			UOSInt GetFontCount();
			WorkbookFont *GetFont(UOSInt index);
			UOSInt GetFontIndex(WorkbookFont *font);
			WorkbookFont *NewFont(const UTF8Char *name, Double size, Bool bold);

			static void GetDefPalette(UInt32 *palette);
			static UTF8Char *ColCode(UTF8Char *sbuff, UOSInt col);
		};
	}
}
#endif
