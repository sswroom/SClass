#ifndef _SM_TEXT_REPORTBUILDER
#define _SM_TEXT_REPORTBUILDER
#include "Data/ArrayListArr.h"
#include "Data/Chart.h"
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
			UOSInt col;
			UOSInt row;
			Double lat;
			Double lon;
		} ColURLLatLon;

		typedef struct
		{
			UOSInt col;
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
			UOSInt nameCellCnt;
			NN<Text::String> value;
			UOSInt valueCellCnt;
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
		UOSInt colCount;
		Data::ArrayListNN<HeaderInfo> preheaders;
		Data::ArrayListNN<HeaderInfo> headers;
		Data::ArrayListArr<TableCell> tableContent;
		Data::ArrayList<RowType> tableRowType;
		Data::ArrayListNN<ColURLLatLon> urlList;
		Data::ArrayList<Optional<Data::ArrayListNN<ColIcon>>> icons;
		Bool tableBorders;
		Double *colWidthPts;
		ColType *colTypes;
		Data::Chart *chart;
		Bool paperHori;

		static Text::SpreadSheet::AxisType FromChartDataType(Data::Chart::DataType dataType);
	public:
		ReportBuilder(Text::CString name, UOSInt colCount, UnsafeArray<UnsafeArrayOpt<const UTF8Char>> columns);
		~ReportBuilder();

		void SetNameHAlign(Text::HAlignment hAlign);
		void SetTableBorders(Bool borders);
		void SetFontName(Text::String *fontName);
		void SetFontName(Text::CString fontName);
		void SetPaperHori(Bool paperHori);
		void AddChart(Data::Chart *chart);
		void AddPreHeader(Text::CString name, Text::CString val);
		void AddPreHeader(Text::CString name, UOSInt nameCellCnt, Text::CString val, UOSInt valCellCnt, Bool valUnderline, Bool right);
		void AddHeader(Text::CString name, Text::CString val);
		void AddHeader(Text::CString name, UOSInt nameCellCnt, Text::CString val, UOSInt valCellCnt, Bool valUnderline, Bool right);
		void AddTableHeader(UnsafeArray<UnsafeArrayOpt<const UTF8Char>> content);
		void AddTableContent(UnsafeArray<UnsafeArrayOpt<const UTF8Char>> content);
		void AddTableSummary(UnsafeArray<UnsafeArrayOpt<const UTF8Char>> content);
		void AddIcon(UOSInt index, Text::CString fileName, Text::CString name);
		void SetColumnWidthPts(UOSInt index, Double width);
		void SetColumnType(UOSInt index, ColType colType);
		void SetColURLLatLon(UOSInt index, Math::Coord2DDbl pos);
		void SetColHAlign(UOSInt index, HAlignment hAlign);
		Bool HasChart();
		NN<Text::SpreadSheet::Workbook> CreateWorkbook();
		NN<Media::VectorDocument> CreateVDoc(Int32 id, NN<Media::DrawEngine> deng);
	};
}
#endif
