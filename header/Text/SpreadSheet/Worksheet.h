#ifndef _SM_TEXT_SPREADSHEET_WORKSHEET
#define _SM_TEXT_SPREADSHEET_WORKSHEET
#include "Data/ArrayList.h"
#include "Data/ArrayListDbl.h"
#include "Data/DateTime.h"
#include "Math/Unit/Distance.h"
#include "Text/String.h"
#include "Text/SpreadSheet/CellStyle.h"
#include "Text/SpreadSheet/IStyleCtrl.h"
#include "Text/SpreadSheet/OfficeChart.h"

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
			MergedUp
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
			OfficeChart *chart;
		};

		class Worksheet
		{
		public:
			typedef struct
			{
				CellDataType cdt;
				Text::String *cellValue;
				CellStyle *style;
				UInt32 mergeHori;
				UInt32 mergeVert;
				Bool hidden;
				Text::String *cellURL;
			} CellData;

			typedef struct
			{
				CellStyle *style;
				Data::ArrayList<CellData*> *cells;
				Double height;
			} RowData;

		private:
			Text::String *name;
			Data::ArrayList<RowData*> *rows;
			Data::ArrayListDbl *colWidthsPt;
			Data::ArrayList<WorksheetDrawing*> *drawings;
			Double defColWidthPt;
			Double defRowHeightPt;
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
			UOSInt maxCol;

		public:
			RowData *CreateRow(UOSInt row);
		private:
			CellData *GetCellData(UOSInt row, UOSInt col, Bool keepMerge);
			void FreeRowData(RowData *data);
			void FreeCellData(CellData *data);
			RowData *CloneRow(RowData *row, const IStyleCtrl *srcCtrl, IStyleCtrl *newCtrl);
			CellData *CloneCell(CellData *cell, const IStyleCtrl *srcCtrl, IStyleCtrl *newCtrl);
			void FreeDrawing(WorksheetDrawing *drawing);
		public:
			Worksheet(Text::String *name);
			Worksheet(Text::CString name);
			~Worksheet();

			Worksheet *Clone(const IStyleCtrl *srcCtrl, IStyleCtrl *newCtrl);

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
			void SetDefColWidthPt(Double defColWidthPt);
			Double GetDefColWidthPt();
			void SetDefRowHeightPt(Double defRowHeightPt);
			Double GetDefRowHeightPt();

			Text::String *GetName();
			Bool SetCellString(UOSInt row, UOSInt col, Text::String *val);
			Bool SetCellString(UOSInt row, UOSInt col, Text::CString val);
			Bool SetCellDate(UOSInt row, UOSInt col, Data::DateTime *val);
			Bool SetCellDouble(UOSInt row, UOSInt col, Double val);
			Bool SetCellInt32(UOSInt row, UOSInt col, Int32 val);
			Bool SetCellStyle(UOSInt row, UOSInt col, CellStyle *style);
			Bool SetCellStyleHAlign(UOSInt row, UOSInt col, IStyleCtrl *wb, HAlignment hAlign);
			Bool SetCellStyleBorderBottom(UOSInt row, UOSInt col, IStyleCtrl *wb, UInt32 color, BorderType borderType);
			Bool SetCellURL(UOSInt row, UOSInt col, Text::String *url);
			Bool SetCellURL(UOSInt row, UOSInt col, Text::CString url);
			Bool SetCellString(UOSInt row, UOSInt col, CellStyle *style, Text::String *val);
			Bool SetCellString(UOSInt row, UOSInt col, CellStyle *style, Text::CString val);
			Bool SetCellDate(UOSInt row, UOSInt col, CellStyle *style, Data::DateTime *val);
			Bool SetCellDouble(UOSInt row, UOSInt col, CellStyle *style, Double val);
			Bool SetCellInt32(UOSInt row, UOSInt col, CellStyle *style, Int32 val);
			Bool MergeCells(UOSInt row, UOSInt col, UInt32 height, UInt32 width);
			Bool SetCellMergeLeft(UOSInt row, UOSInt col);
			Bool SetCellMergeUp(UOSInt row, UOSInt col);
			Bool SetRowHidden(UOSInt row, Bool hidden);
			Bool SetRowHeight(UOSInt row, Double height);

			UOSInt GetCount();
			RowData *GetItem(UOSInt row);
			void RemoveCol(UOSInt col);
			void InsertCol(UOSInt col);
			UOSInt GetMaxCol();

			void SetColWidth(UOSInt col, Double width, Math::Unit::Distance::DistanceUnit unit);
			UOSInt GetColWidthCount();
			Double GetColWidthPt(UOSInt col);
			Double GetColWidth(UOSInt col, Math::Unit::Distance::DistanceUnit unit);

			const CellData *GetCellDataRead(UOSInt row, UOSInt col) const;

			UOSInt GetDrawingCount();
			WorksheetDrawing *GetDrawing(UOSInt index);
			WorksheetDrawing *CreateDrawing(Math::Unit::Distance::DistanceUnit unit, Double x, Double y, Double w, Double h);
			OfficeChart *CreateChart(Math::Unit::Distance::DistanceUnit unit, Double x, Double y, Double w, Double h, Text::CString title);
		};
	}
}

#endif
