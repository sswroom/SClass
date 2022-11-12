#ifndef _SM_TEXT_REPORTBUILDER
#define _SM_TEXT_REPORTBUILDER
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

		static Text::CString ColumnMergeLeft;
		static Text::CString ColumnMergeUp;
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
			Media::DrawImage *dimg;
		} IconStatus;

		struct HeaderInfo
		{
			Text::String *name;
			UOSInt nameCellCnt;
			Text::String *value;
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
		Text::String *name;
		Text::HAlignment nameHAlign;
		Text::String *fontName;
		UOSInt colCount;
		Data::ArrayList<HeaderInfo*> preheaders;
		Data::ArrayList<HeaderInfo*> headers;
		Data::ArrayList<TableCell*> tableContent;
		Data::ArrayList<RowType> tableRowType;
		Data::ArrayList<ColURLLatLon*> urlList;
		Data::ArrayList<Data::ArrayList<ColIcon*>*> icons;
		Bool tableBorders;
		Double *colWidthPts;
		ColType *colTypes;
		Data::Chart *chart;
		Bool paperHori;

		static Text::SpreadSheet::AxisType FromChartDataType(Data::Chart::DataType dataType);
	public:
		ReportBuilder(Text::CString name, UOSInt colCount, const UTF8Char **columns);
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
		void AddTableHeader(const UTF8Char **content);
		void AddTableContent(const UTF8Char **content);
		void AddTableSummary(const UTF8Char **content);
		void AddIcon(UOSInt index, Text::CString fileName, Text::CString name);
		void SetColumnWidthPts(UOSInt index, Double width);
		void SetColumnType(UOSInt index, ColType colType);
		void SetColURLLatLon(UOSInt index, Double lat, Double lon);
		void SetColHAlign(UOSInt index, HAlignment hAlign);
		Bool HasChart();
		Text::SpreadSheet::Workbook *CreateWorkbook();
		Media::VectorDocument *CreateVDoc(Int32 id, Media::DrawEngine *deng);
	};
}
#endif
