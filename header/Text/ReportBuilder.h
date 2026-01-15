#ifndef _SM_TEXT_REPORTBUILDER
#define _SM_TEXT_REPORTBUILDER
#include "Data/ArrayListArr.hpp"
#include "Data/ChartPlotter.h"
#include "Media/VectorDocument.h"
#include "Text/String.h"
#include "Text/TextCommon.h"
#include "Text/SpreadSheet/Workbook.h"

namespace Text
{
	class ReportBuilder
	{
	public:
		typedef enum
		{
			CT_STRING,
			CT_DOUBLE,
			CT_INT32
		} ColType;

		static Text::CStringNN ColumnMergeLeft;
		static Text::CStringNN ColumnMergeUp;
	private:
		typedef enum
		{
			RT_UNKNOWN,
			RT_CONTENT,
			RT_SUMMARY,
			RT_HEADER
		} RowType;

		typedef struct
		{
			UIntOS col;
			UIntOS row;
			Double lat;
			Double lon;
		} ColURLLatLon;

		typedef struct
		{
			UIntOS col;
			Text::String *fileName;
			Text::String *name;
		} ColIcon;

		typedef struct
		{
			Optional<Media::DrawImage> dimg;
		} IconStatus;

		struct HeaderInfo
		{
			NN<Text::String> name;
			UIntOS nameCellCnt;
			NN<Text::String> value;
			UIntOS valueCellCnt;
			Bool valueUnderline;
			Bool isRight;
		};

		struct TableCell
		{
			Text::String *val;
			HAlignment hAlign;
		};
	private:
		NN<Text::String> name;
		Text::HAlignment nameHAlign;
		NN<Text::String> fontName;
		UIntOS colCount;
		Data::ArrayListNN<HeaderInfo> preheaders;
		Data::ArrayListNN<HeaderInfo> headers;
		Data::ArrayListArr<TableCell> tableContent;
		Data::ArrayListNative<RowType> tableRowType;
		Data::ArrayListNN<ColURLLatLon> urlList;
		Data::ArrayListObj<Optional<Data::ArrayListNN<ColIcon>>> icons;
		Bool tableBorders;
		Double *colWidthPts;
		ColType *colTypes;
		Data::ChartPlotter *chart;
		Bool paperHori;

		static Text::SpreadSheet::AxisType FromChartDataType(Data::ChartPlotter::DataType dataType);
	public:
		ReportBuilder(Text::CStringNN name, UIntOS colCount, UnsafeArray<UnsafeArrayOpt<const UTF8Char>> columns);
		~ReportBuilder();

		void SetNameHAlign(Text::HAlignment hAlign);
		void SetTableBorders(Bool borders);
		void SetFontName(NN<Text::String> fontName);
		void SetFontName(Text::CStringNN fontName);
		void SetPaperHori(Bool paperHori);
		void AddChart(Data::ChartPlotter *chart);
		void AddPreHeader(Text::CStringNN name, Text::CStringNN val);
		void AddPreHeader(Text::CStringNN name, UIntOS nameCellCnt, Text::CStringNN val, UIntOS valCellCnt, Bool valUnderline, Bool right);
		void AddHeader(Text::CStringNN name, Text::CStringNN val);
		void AddHeader(Text::CStringNN name, UIntOS nameCellCnt, Text::CStringNN val, UIntOS valCellCnt, Bool valUnderline, Bool right);
		void AddTableHeader(UnsafeArray<UnsafeArrayOpt<const UTF8Char>> content);
		void AddTableContent(UnsafeArray<UnsafeArrayOpt<const UTF8Char>> content);
		void AddTableSummary(UnsafeArray<UnsafeArrayOpt<const UTF8Char>> content);
		void AddIcon(UIntOS index, Text::CString fileName, Text::CString name);
		void SetColumnWidthPts(UIntOS index, Double width);
		void SetColumnType(UIntOS index, ColType colType);
		void SetColURLLatLon(UIntOS index, Math::Coord2DDbl pos);
		void SetColHAlign(UIntOS index, HAlignment hAlign);
		Bool HasChart();
		NN<Text::SpreadSheet::Workbook> CreateWorkbook();
		NN<Media::VectorDocument> CreateVDoc(Int32 id, NN<Media::DrawEngine> deng);
	};
}
#endif
