#ifndef _SM_TEXT_SPREADSHEET_WORKSHEET
#define _SM_TEXT_SPREADSHEET_WORKSHEET
#include "Data/ArrayList.h"
#include "Data/ArrayListDbl.h"
#include "Data/DateTime.h"
#include "Math/Unit/Distance.h"
#include "Text/SpreadSheet/CellStyle.h"
#include "Text/SpreadSheet/IStyleCtrl.h"

namespace Text
{
	namespace SpreadSheet
	{
		enum class CellDataType
		{
			String,
			Number,
			DateTime,
			MergedLeft,
			MergedTop
		};

		enum class AnchorType
		{
			Absolute,
			OneCell,
			TwoCell
		};

		struct WorksheetDrawing
		{
			AnchorType anchorType;
			Double posXInch;
			Double posYInch;
			Double widthInch;
			Double heightInch;
			UOSInt row1;
			UOSInt col1;
			UOSInt row2;
			UOSInt col2;
		};

		class Worksheet
		{
		public:
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
			Data::ArrayList<WorksheetDrawing*> *drawings;
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
			void FreeDrawing(WorksheetDrawing *drawing);
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
			Bool SetCellString(UOSInt row, UOSInt col, CellStyle *style, const UTF8Char *val);
			Bool SetCellDate(UOSInt row, UOSInt col, CellStyle *style, Data::DateTime *val);
			Bool SetCellDouble(UOSInt row, UOSInt col, CellStyle *style, Double val);
			Bool SetCellInt32(UOSInt row, UOSInt col, CellStyle *style, Int32 val);
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

			UOSInt GetDrawingCount();
			WorksheetDrawing *GetDrawing(UOSInt index);
			WorksheetDrawing *CreateDrawing(Math::Unit::Distance::DistanceUnit unit, Double x, Double y, Double w, Double h);
			void CreateChart(Math::Unit::Distance::DistanceUnit unit, Double x, Double y, Double w, Double h, const UTF8Char *title);
		};
	}
}

#endif
