#ifndef _SM_TEXT_SPREADSHEET_WORKBOOK
#define _SM_TEXT_SPREADSHEET_WORKBOOK
#include "Data/DateTime.h"
#include "Data/ArrayListNN.hpp"
#include "IO/ParsedObject.h"
#include "Text/SpreadSheet/SpreadSheetStyleCtrl.h"
#include "Text/SpreadSheet/WorkbookFont.h"
#include "Text/SpreadSheet/Worksheet.h"

namespace Text
{
	namespace SpreadSheet
	{
		class Workbook : public IO::ParsedObject, public Text::SpreadSheet::SpreadSheetStyleCtrl
		{
		private:
			UnsafeArrayOpt<const UTF8Char> author;
			UnsafeArrayOpt<const UTF8Char> lastAuthor;
			UnsafeArrayOpt<const UTF8Char> company;
			Data::DateTime *createTime;
			Data::DateTime *modifyTime;
			Double version;
			Int32 windowTopX;
			Int32 windowTopY;
			Int32 windowWidth;
			Int32 windowHeight;
			UIntOS activeSheet;
			UInt32 palette[56];

			Data::ArrayListNN<Worksheet> sheets;
			Data::ArrayListNN<CellStyle> styles;
			Data::ArrayListNN<WorkbookFont> fonts;

			static const UInt32 defPalette[56];
		public:
			Workbook();
			virtual ~Workbook();

			virtual IO::ParserType GetParserType() const;

			NN<Workbook> Clone() const;

			void AddDefaultStyles();

			void SetAuthor(UnsafeArrayOpt<const UTF8Char> author);
			void SetLastAuthor(UnsafeArrayOpt<const UTF8Char> lastAuthor);
			void SetCompany(UnsafeArrayOpt<const UTF8Char> company);
			void SetCreateTime(Data::DateTime *createTime);
			void SetCreateTime(Data::Timestamp createTime);
			void SetModifyTime(Data::DateTime *modifyTime);
			void SetModifyTime(Data::Timestamp modifyTime);
			void SetVersion(Double version);
			UnsafeArrayOpt<const UTF8Char> GetAuthor() const;
			UnsafeArrayOpt<const UTF8Char> GetLastAuthor() const;
			UnsafeArrayOpt<const UTF8Char> GetCompany() const;
			Data::DateTime *GetCreateTime() const;
			Data::DateTime *GetModifyTime() const;
			Double GetVersion() const;
			Bool HasInfo() const;

			void SetWindowTopX(Int32 windowTopX);
			void SetWindowTopY(Int32 windowTopY);
			void SetWindowWidth(Int32 windowWidth);
			void SetWindowHeight(Int32 windowHeight);
			void SetActiveSheet(UIntOS index);
			Int32 GetWindowTopX();
			Int32 GetWindowTopY();
			Int32 GetWindowWidth();
			Int32 GetWindowHeight();
			UIntOS GetActiveSheet();
			Bool HasWindowInfo();

			Bool HasCellStyle();
			NN<CellStyle> NewCellStyle();
			NN<CellStyle> NewCellStyle(Optional<WorkbookFont> font, HAlignment halign, VAlignment valign, Text::CString dataFormat);
			UIntOS GetStyleCount() const;
			virtual IntOS GetStyleIndex(NN<CellStyle> style) const;
			virtual Optional<CellStyle> GetStyle(UIntOS index) const;
			virtual NN<CellStyle> FindOrCreateStyle(NN<const CellStyle> tmpStyle);
			Data::ArrayIterator<NN<CellStyle>> StyleIterator() const;
			Optional<CellStyle> GetDefaultStyle();
			void GetPalette(UInt32 *palette);
			void SetPalette(UInt32 *palette);

			NN<Worksheet> AddWorksheet();
			NN<Worksheet> AddWorksheet(NN<Text::String> name);
			NN<Worksheet> AddWorksheet(Text::CStringNN name);
			NN<Worksheet> InsertWorksheet(UIntOS index, Text::CStringNN name);
			UIntOS GetCount() const;
			Optional<Worksheet> GetItem(UIntOS index) const;
			Data::ArrayIterator<NN<Worksheet>> Iterator() const;
			void RemoveAt(UIntOS index);
			Optional<Worksheet> GetWorksheetByName(Text::CStringNN name);

			UIntOS GetFontCount() const;
			NN<WorkbookFont> GetFontNoCheckc(UIntOS index) const;
			Optional<WorkbookFont> GetFont(UIntOS index) const;
			UIntOS GetFontIndex(NN<WorkbookFont> font);
			NN<WorkbookFont> NewFont(Text::CString name, Double size, Bool bold);

			static void GetDefPalette(UInt32 *palette);
			static UnsafeArray<UTF8Char> ColCode(UnsafeArray<UTF8Char> sbuff, UIntOS col);
		};
	}
}
#endif
