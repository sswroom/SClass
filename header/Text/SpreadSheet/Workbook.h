#ifndef _SM_TEXT_SPREADSHEET_WORKBOOK
#define _SM_TEXT_SPREADSHEET_WORKBOOK
#include "Data/DateTime.h"
#include "Data/ArrayListNN.h"
#include "IO/ParsedObject.h"
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

			Data::ArrayListNN<Worksheet> sheets;
			Data::ArrayListNN<CellStyle> styles;
			Data::ArrayList<WorkbookFont*> fonts;

			static const UInt32 defPalette[56];
		public:
			Workbook();
			virtual ~Workbook();

			virtual IO::ParserType GetParserType() const;

			Workbook *Clone() const;

			void AddDefaultStyles();

			void SetAuthor(const UTF8Char *author);
			void SetLastAuthor(const UTF8Char *lastAuthor);
			void SetCompany(const UTF8Char *company);
			void SetCreateTime(Data::DateTime *createTime);
			void SetCreateTime(Data::Timestamp createTime);
			void SetModifyTime(Data::DateTime *modifyTime);
			void SetModifyTime(Data::Timestamp modifyTime);
			void SetVersion(Double version);
			const UTF8Char *GetAuthor() const;
			const UTF8Char *GetLastAuthor() const;
			const UTF8Char *GetCompany() const;
			Data::DateTime *GetCreateTime() const;
			Data::DateTime *GetModifyTime() const;
			Double GetVersion() const;
			Bool HasInfo() const;

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
			NN<CellStyle> NewCellStyle();
			NN<CellStyle> NewCellStyle(WorkbookFont *font, HAlignment halign, VAlignment valign, Text::CString dataFormat);
			UOSInt GetStyleCount() const;
			virtual OSInt GetStyleIndex(NN<CellStyle> style) const;
			virtual Optional<CellStyle> GetStyle(UOSInt index) const;
			virtual NN<CellStyle> FindOrCreateStyle(NN<const CellStyle> tmpStyle);
			Data::ArrayIterator<NN<CellStyle>> StyleIterator() const;
			Optional<CellStyle> GetDefaultStyle();
			void GetPalette(UInt32 *palette);
			void SetPalette(UInt32 *palette);

			NN<Worksheet> AddWorksheet();
			NN<Worksheet> AddWorksheet(NN<Text::String> name);
			NN<Worksheet> AddWorksheet(Text::CStringNN name);
			NN<Worksheet> InsertWorksheet(UOSInt index, Text::CStringNN name);
			UOSInt GetCount() const;
			Optional<Worksheet> GetItem(UOSInt index) const;
			Data::ArrayIterator<NN<Worksheet>> Iterator() const;
			void RemoveAt(UOSInt index);
			Optional<Worksheet> GetWorksheetByName(Text::CString name);

			UOSInt GetFontCount();
			WorkbookFont *GetFont(UOSInt index);
			UOSInt GetFontIndex(WorkbookFont *font);
			WorkbookFont *NewFont(Text::CString name, Double size, Bool bold);

			static void GetDefPalette(UInt32 *palette);
			static UTF8Char *ColCode(UTF8Char *sbuff, UOSInt col);
		};
	}
}
#endif
