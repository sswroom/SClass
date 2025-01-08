#ifndef _SM_TEXT_SPREADSHEET_WORKSHEET
#define _SM_TEXT_SPREADSHEET_WORKSHEET
#include "Data/ArrayList.h"
#include "Data/ArrayListDbl.h"
#include "Data/DateTime.h"
#include "Math/Unit/Distance.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"
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
			Optional<OfficeChart> chart;
		};

		class Worksheet
		{
		public:
			typedef struct
			{
				CellDataType cdt;
				Optional<Text::String> cellValue;
				Optional<CellStyle> style;
				UInt32 mergeHori;
				UInt32 mergeVert;
				Bool hidden;
				Optional<Text::String> cellURL;
			} CellData;

			typedef struct
			{
				Optional<CellStyle> style;
				Data::ArrayList<Optional<CellData>> cells;
				Double height;
			} RowData;

		private:
			NN<Text::String> name;
			Data::ArrayList<Optional<RowData>> rows;
			Data::ArrayListDbl colWidthsPt;
			Data::ArrayListNN<WorksheetDrawing> drawings;
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
			Optional<RowData> CreateRow(UOSInt row);
		private:
			Optional<CellData> GetCellData(UOSInt row, UOSInt col, Bool keepMerge);
			void FreeRowData(NN<RowData> data);
			void FreeCellData(NN<CellData> data);
			NN<RowData> CloneRow(NN<RowData> row, NN<const IStyleCtrl> srcCtrl, NN<IStyleCtrl> newCtrl);
			NN<CellData> CloneCell(NN<CellData> cell, NN<const IStyleCtrl> srcCtrl, NN<IStyleCtrl> newCtrl);
			static void __stdcall FreeDrawing(NN<WorksheetDrawing> drawing);
		public:
			Worksheet(NotNullPtr<Text::String> name);
			Worksheet(Text::CStringNN name);
			~Worksheet();

			NotNullPtr<Worksheet> Clone(NN<const IStyleCtrl> srcCtrl, NN<IStyleCtrl> newCtrl);

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

			NotNullPtr<Text::String> GetName() const;
			Bool SetCellString(UOSInt row, UOSInt col, NotNullPtr<Text::String> val);
			Bool SetCellString(UOSInt row, UOSInt col, Text::CStringNN val);
			Bool SetCellTS(UOSInt row, UOSInt col, Data::Timestamp val);
			Bool SetCellDateTime(UOSInt row, UOSInt col, NotNullPtr<Data::DateTime> val);
			Bool SetCellDouble(UOSInt row, UOSInt col, Double val);
			Bool SetCellInt32(UOSInt row, UOSInt col, Int32 val);
			Bool SetCellInt64(UOSInt row, UOSInt col, Int64 val);
			Bool SetCellEmpty(UOSInt row, UOSInt col);
			Bool SetCellStyle(UOSInt row, UOSInt col, Optional<CellStyle> style);
			Bool SetCellStyleHAlign(UOSInt row, UOSInt col, NN<IStyleCtrl> wb, HAlignment hAlign);
			Bool SetCellStyleBorderBottom(UOSInt row, UOSInt col, NN<IStyleCtrl> wb, UInt32 color, BorderType borderType);
			Bool SetCellURL(UOSInt row, UOSInt col, Optional<Text::String> url);
			Bool SetCellURL(UOSInt row, UOSInt col, Text::CString url);
			Bool SetCellString(UOSInt row, UOSInt col, Optional<CellStyle> style, NotNullPtr<Text::String> val);
			Bool SetCellString(UOSInt row, UOSInt col, Optional<CellStyle> style, Text::CStringNN val);
			Bool SetCellTS(UOSInt row, UOSInt col, Optional<CellStyle> style, Data::Timestamp val);
			Bool SetCellDateTime(UOSInt row, UOSInt col, Optional<CellStyle> style, NotNullPtr<Data::DateTime> val);
			Bool SetCellDouble(UOSInt row, UOSInt col, Optional<CellStyle> style, Double val);
			Bool SetCellInt32(UOSInt row, UOSInt col, Optional<CellStyle> style, Int32 val);
			Bool SetCellInt64(UOSInt row, UOSInt col, Optional<CellStyle> style, Int64 val);
			Bool SetCellEmpty(UOSInt row, UOSInt col, Optional<CellStyle> style);
			Bool MergeCells(UOSInt row, UOSInt col, UInt32 height, UInt32 width);
			Bool SetCellMergeLeft(UOSInt row, UOSInt col);
			Bool SetCellMergeUp(UOSInt row, UOSInt col);
			Bool SetRowHidden(UOSInt row, Bool hidden);
			Bool SetRowHeight(UOSInt row, Double height);

			UOSInt GetCount();
			Optional<RowData> GetItem(UOSInt row);
			void RemoveCol(UOSInt col);
			void InsertCol(UOSInt col);
			UOSInt GetMaxCol();

			void SetColWidth(UOSInt col, Double width, Math::Unit::Distance::DistanceUnit unit);
			UOSInt GetColWidthCount();
			Double GetColWidthPt(UOSInt col);
			Double GetColWidth(UOSInt col, Math::Unit::Distance::DistanceUnit unit);

			Optional<const CellData> GetCellDataRead(UOSInt row, UOSInt col) const;
			Bool GetCellString(const CellData *cell, NotNullPtr<Text::StringBuilderUTF8> sb);

			UOSInt GetDrawingCount();
			Optional<WorksheetDrawing> GetDrawing(UOSInt index);
			NN<WorksheetDrawing> GetDrawingNoCheck(UOSInt index);
			NN<WorksheetDrawing> CreateDrawing(Math::Unit::Distance::DistanceUnit unit, Double x, Double y, Double w, Double h);
			NN<OfficeChart> CreateChart(Math::Unit::Distance::DistanceUnit unit, Double x, Double y, Double w, Double h, Text::CString title);
		};

		Text::CString CellDataTypeGetName(CellDataType cdt);
	}
}

#endif
