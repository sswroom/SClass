#ifndef _SM_TEXT_REPORTBUILDER
#define _SM_TEXT_REPORTBUILDER
#include "Data/IChart.h"
#include "Media/VectorDocument.h"
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
			const UTF8Char *fileName;
			const UTF8Char *name;
		} ColIcon;

		typedef struct
		{
			Media::DrawImage *dimg;
		} IconStatus;
	private:
		const UTF8Char *name;
		const UTF8Char *fontName;
		UOSInt colCount;
		Data::ArrayList<const UTF8Char **> *preheaders;
		Data::ArrayList<const UTF8Char **> *headers;
		Data::ArrayList<const UTF8Char **> *tableContent;
		Data::ArrayList<RowType> *tableRowType;
		Data::ArrayList<ColURLLatLon*> *urlList;
		Data::ArrayList<Data::ArrayList<ColIcon*>*> *icons;
		Double *colWidth;
		ColType *colTypes;
		Data::IChart *chart;
		Bool paperHori;

	public:
		ReportBuilder(const UTF8Char *name, UOSInt colCount, const UTF8Char **columns);
		~ReportBuilder();

		void SetFontName(const UTF8Char *fontName);
		void SetPaperHori(Bool paperHori);
		void AddChart(Data::IChart *chart);
		void AddPreHeader(const UTF8Char *name, const UTF8Char *val);
		void AddHeader(const UTF8Char *name, const UTF8Char *val);
		void AddTableContent(const UTF8Char **content);
		void AddTableSummary(const UTF8Char **content);
		void AddIcon(UOSInt index, const UTF8Char *fileName, const UTF8Char *name);
		void SetColumnWidth(UOSInt index, Double width);
		void SetColumnType(UOSInt index, ColType colType);
		void SetColURLLatLon(UOSInt index, Double lat, Double lon);
		Text::SpreadSheet::Workbook *CreateWorkbook();
		Media::VectorDocument *CreateVDoc(Int32 id, Media::DrawEngine *deng);
	};
}
#endif
