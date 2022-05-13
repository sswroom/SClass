#ifndef _SM_TEXT_REPORTBUILDER
#define _SM_TEXT_REPORTBUILDER
#include "Data/IChart.h"
#include "Media/VectorDocument.h"
#include "Text/String.h"
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
	private:
		Text::String *name;
		Text::String *fontName;
		UOSInt colCount;
		Data::ArrayList<Text::String **> preheaders;
		Data::ArrayList<Text::String **> headers;
		Data::ArrayList<Text::String **> tableContent;
		Data::ArrayList<RowType> tableRowType;
		Data::ArrayList<ColURLLatLon*> urlList;
		Data::ArrayList<Data::ArrayList<ColIcon*>*> icons;
		Double *colWidthPts;
		ColType *colTypes;
		Data::IChart *chart;
		Bool paperHori;

		static Text::SpreadSheet::AxisType FromChartDataType(Data::IChart::DataType dataType);
	public:
		ReportBuilder(Text::CString name, UOSInt colCount, const UTF8Char **columns);
		~ReportBuilder();

		void SetFontName(Text::String *fontName);
		void SetFontName(Text::CString fontName);
		void SetPaperHori(Bool paperHori);
		void AddChart(Data::IChart *chart);
		void AddPreHeader(Text::CString name, Text::CString val);
		void AddHeader(Text::CString name, Text::CString val);
		void AddTableContent(const UTF8Char **content);
		void AddTableSummary(const UTF8Char **content);
		void AddIcon(UOSInt index, Text::CString fileName, Text::CString name);
		void SetColumnWidthPts(UOSInt index, Double width);
		void SetColumnType(UOSInt index, ColType colType);
		void SetColURLLatLon(UOSInt index, Double lat, Double lon);
		Bool HasChart();
		Text::SpreadSheet::Workbook *CreateWorkbook();
		Media::VectorDocument *CreateVDoc(Int32 id, Media::DrawEngine *deng);
	};
}
#endif
