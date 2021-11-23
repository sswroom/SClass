#ifndef _SM_TEXT_SPREADSHEET_WORKSHEET
#define _SM_TEXT_SPREADSHEET_WORKSHEET
#include "Data/ArrayList.h"
#include "Data/ArrayListDbl.h"
#include "Data/DateTime.h"
#include "Text/SpreadSheet/CellStyle.h"
#include "Text/SpreadSheet/IStyleCtrl.h"

namespace Text
{
	namespace SpreadSheet
	{
		class Worksheet
		{
		public:
			typedef enum
			{
				CDT_STRING,
				CDT_NUMBER,
				CDT_DATETIME,
				CDT_MERGEDLEFT,
				CDT_MERGEDTOP
			} CellDataType;

			typedef struct
			{
				CellDataType cdt;
				const UTF8Char *cellValue;
				CellStyle *style;
				UInt32 mergeHori;
				UInt32 mergeVert;
				Bool hidden;
				const UTF8Char *cellURL;
			} CellData;

			typedef struct
			{
				CellStyle *style;
				Data::ArrayList<CellData*> *cells;
				Double height;
			} RowData;
		private:
			const UTF8Char *name;
			Data::ArrayList<RowData*> *rows;
			Data::ArrayListDbl *colWidths;
			UInt32 freezeHori;
			UInt32 freezeVert;
			Double marginLeft;
			Double marginRight;
			Double marginTop;
			Double marginBottom;
			Double marginHeader;
			Double marginFooter;
			UInt16 options;
			UInt32 zoom;

		public:
			RowData *CreateRow(UOSInt row);
		private:
			CellData *GetCellData(UOSInt row, UOSInt col, Bool keepMerge);
			void FreeRowData(RowData *data);
			void FreeCellData(CellData *data);
			RowData *CloneRow(RowData *row, IStyleCtrl *srcCtrl, IStyleCtrl *newCtrl);
			CellData *CloneCell(CellData *cell, IStyleCtrl *srcCtrl, IStyleCtrl *newCtrl);
		public:
			Worksheet(const UTF8Char *name);
			~Worksheet();

			Worksheet *Clone(IStyleCtrl *srcCtrl, IStyleCtrl *newCtrl);

			void SetOptions(UInt16 options);
			UInt16 GetOptions();
			void SetFreezeHori(UInt32 freezeHori);
			UInt32 GetFreezeHori();
			void SetFreezeVert(UInt32 freezeVert);
			UInt32 GetFreezeVert();
			void SetMarginLeft(Double marginLeft); //Inch
			Double GetMarginLeft();
			void SetMarginRight(Double marginRight); //Inch
			Double GetMarginRight();
			void SetMarginTop(Double marginTop); //Inch
			Double GetMarginTop();
			void SetMarginBottom(Double marginBottom); //Inch
			Double GetMarginBottom();
			void SetMarginHeader(Double marginHeader); //Inch
			Double GetMarginHeader();
			void SetMarginFooter(Double marginFooter); //Inch
			Double GetMarginFooter();
			void SetZoom(UInt32 zoom);
			UInt32 GetZoom();
			Bool IsDefaultPageSetup();

			const UTF8Char *GetName();
			Bool SetCellString(UOSInt row, UOSInt col, const UTF8Char *val);
			Bool SetCellDate(UOSInt row, UOSInt col, Data::DateTime *val);
			Bool SetCellDouble(UOSInt row, UOSInt col, Double val);
			Bool SetCellInt32(UOSInt row, UOSInt col, Int32 val);
			Bool SetCellStyle(UOSInt row, UOSInt col, CellStyle *style);
			Bool SetCellURL(UOSInt row, UOSInt col, const UTF8Char *url);
			Bool MergeCells(UOSInt row, UOSInt col, UInt32 height, UInt32 width);
			Bool SetRowHidden(UOSInt row, Bool hidden);
			Bool SetRowHeight(UOSInt row, Double height);

			UOSInt GetCount();
			RowData *GetItem(UOSInt row);
			void RemoveCol(UOSInt col);
			void InsertCol(UOSInt col);

			void SetColWidth(UOSInt col, Double width);
			UOSInt GetColWidthCount();
			Double GetColWidth(UOSInt col);

			static void Number2Time(Data::DateTime *dt, Double number);
		};
	}
}

#endif
