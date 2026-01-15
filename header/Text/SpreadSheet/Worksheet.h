#ifndef _SM_TEXT_SPREADSHEET_WORKSHEET
#define _SM_TEXT_SPREADSHEET_WORKSHEET
#include "Data/ArrayListDbl.h"
#include "Data/ArrayListObj.hpp"
#include "Data/DateTime.h"
#include "Math/Unit/Distance.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/SpreadSheet/CellStyle.h"
#include "Text/SpreadSheet/SpreadSheetStyleCtrl.h"
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
			UIntOS row1;
			UIntOS col1;
			UIntOS row2;
			UIntOS col2;
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
				Data::ArrayListObj<Optional<CellData>> cells;
				Double height;
			} RowData;

		private:
			NN<Text::String> name;
			Data::ArrayListObj<Optional<RowData>> rows;
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
			UIntOS maxCol;

		public:
			Optional<RowData> CreateRow(UIntOS row);
		private:
			Optional<CellData> GetCellData(UIntOS row, UIntOS col, Bool keepMerge);
			void FreeRowData(NN<RowData> data);
			void FreeCellData(NN<CellData> data);
			NN<RowData> CloneRow(NN<RowData> row, NN<const SpreadSheetStyleCtrl> srcCtrl, NN<SpreadSheetStyleCtrl> newCtrl);
			NN<CellData> CloneCell(NN<CellData> cell, NN<const SpreadSheetStyleCtrl> srcCtrl, NN<SpreadSheetStyleCtrl> newCtrl);
			static void __stdcall FreeDrawing(NN<WorksheetDrawing> drawing);
		public:
			Worksheet(NotNullPtr<Text::String> name);
			Worksheet(Text::CStringNN name);
			~Worksheet();

			NotNullPtr<Worksheet> Clone(NN<const SpreadSheetStyleCtrl> srcCtrl, NN<SpreadSheetStyleCtrl> newCtrl);

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
			Bool SetCellString(UIntOS row, UIntOS col, NotNullPtr<Text::String> val);
			Bool SetCellString(UIntOS row, UIntOS col, Text::CStringNN val);
			Bool SetCellTS(UIntOS row, UIntOS col, Data::Timestamp val);
			Bool SetCellDateTime(UIntOS row, UIntOS col, NotNullPtr<Data::DateTime> val);
			Bool SetCellDouble(UIntOS row, UIntOS col, Double val);
			Bool SetCellInt32(UIntOS row, UIntOS col, Int32 val);
			Bool SetCellInt64(UIntOS row, UIntOS col, Int64 val);
			Bool SetCellEmpty(UIntOS row, UIntOS col);
			Bool SetCellStyle(UIntOS row, UIntOS col, Optional<CellStyle> style);
			Bool SetCellStyleHAlign(UIntOS row, UIntOS col, NN<SpreadSheetStyleCtrl> wb, HAlignment hAlign);
			Bool SetCellStyleBorderBottom(UIntOS row, UIntOS col, NN<SpreadSheetStyleCtrl> wb, UInt32 color, BorderType borderType);
			Bool SetCellURL(UIntOS row, UIntOS col, Optional<Text::String> url);
			Bool SetCellURL(UIntOS row, UIntOS col, Text::CString url);
			Bool SetCellString(UIntOS row, UIntOS col, Optional<CellStyle> style, NotNullPtr<Text::String> val);
			Bool SetCellString(UIntOS row, UIntOS col, Optional<CellStyle> style, Text::CStringNN val);
			Bool SetCellTS(UIntOS row, UIntOS col, Optional<CellStyle> style, Data::Timestamp val);
			Bool SetCellDateTime(UIntOS row, UIntOS col, Optional<CellStyle> style, NotNullPtr<Data::DateTime> val);
			Bool SetCellDouble(UIntOS row, UIntOS col, Optional<CellStyle> style, Double val);
			Bool SetCellInt32(UIntOS row, UIntOS col, Optional<CellStyle> style, Int32 val);
			Bool SetCellInt64(UIntOS row, UIntOS col, Optional<CellStyle> style, Int64 val);
			Bool SetCellEmpty(UIntOS row, UIntOS col, Optional<CellStyle> style);
			Bool MergeCells(UIntOS row, UIntOS col, UInt32 height, UInt32 width);
			Bool SetCellMergeLeft(UIntOS row, UIntOS col);
			Bool SetCellMergeUp(UIntOS row, UIntOS col);
			Bool SetRowHidden(UIntOS row, Bool hidden);
			Bool SetRowHeight(UIntOS row, Double height);

			UIntOS GetCount();
			Optional<RowData> GetItem(UIntOS row);
			void RemoveCol(UIntOS col);
			void InsertCol(UIntOS col);
			UIntOS GetMaxCol();

			void SetColWidth(UIntOS col, Double width, Math::Unit::Distance::DistanceUnit unit);
			UIntOS GetColWidthCount();
			Double GetColWidthPt(UIntOS col);
			Double GetColWidth(UIntOS col, Math::Unit::Distance::DistanceUnit unit);

			Optional<const CellData> GetCellDataRead(UIntOS row, UIntOS col) const;
			Bool GetCellString(const CellData *cell, NotNullPtr<Text::StringBuilderUTF8> sb);

			UIntOS GetDrawingCount();
			Optional<WorksheetDrawing> GetDrawing(UIntOS index);
			NN<WorksheetDrawing> GetDrawingNoCheck(UIntOS index);
			NN<WorksheetDrawing> CreateDrawing(Math::Unit::Distance::DistanceUnit unit, Double x, Double y, Double w, Double h);
			NN<OfficeChart> CreateChart(Math::Unit::Distance::DistanceUnit unit, Double x, Double y, Double w, Double h, Text::CString title);
		};

		Text::CString CellDataTypeGetName(CellDataType cdt);
	}
}

#endif
