#ifndef _SM_EXPORTER_XLSXEXPORTER
#define _SM_EXPORTER_XLSXEXPORTER
#include "Data/DateTime.h"
#include "IO/FileExporter.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/SpreadSheet/OfficeChartAxis.h"
#include "Text/SpreadSheet/OfficeChartSeries.h"
#include "Text/SpreadSheet/OfficeShapeProp.h"
#include "Text/SpreadSheet/Worksheet.h"

namespace Exporter
{
	class XLSXExporter : public IO::FileExporter
	{
	private:
		struct LinkInfo
		{
			UOSInt row;
			UOSInt col;
			Text::SpreadSheet::Worksheet::CellData *cell;
		};

		struct BorderInfo
		{
			Text::SpreadSheet::CellStyle::BorderStyle left;
			Text::SpreadSheet::CellStyle::BorderStyle top;
			Text::SpreadSheet::CellStyle::BorderStyle right;
			Text::SpreadSheet::CellStyle::BorderStyle bottom;
		};

	public:
		XLSXExporter();
		virtual ~XLSXExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(IO::SeekableStream *stm, Text::CString fileName, IO::ParsedObject *pobj, void *param);

		static void AppendFill(Text::StringBuilderUTF8 *sb, Text::SpreadSheet::OfficeFill *fill);
		static void AppendLineStyle(Text::StringBuilderUTF8 *sb, Text::SpreadSheet::OfficeLineStyle *lineStyle);
		static void AppendTitle(Text::StringBuilderUTF8 *sb, const UTF8Char *title);
		static void AppendShapeProp(Text::StringBuilderUTF8 *sb, Text::SpreadSheet::OfficeShapeProp *shapeProp);
		static void AppendAxis(Text::StringBuilderUTF8 *sb, Text::SpreadSheet::OfficeChartAxis *axis, UOSInt index);
		static void AppendSeries(Text::StringBuilderUTF8 *sb, Text::SpreadSheet::OfficeChartSeries *series, UOSInt index);
		static void AppendBorder(Text::StringBuilderUTF8 *sb, Text::SpreadSheet::CellStyle::BorderStyle border, Text::CString name);

		static Text::CString PresetColorCode(Text::SpreadSheet::PresetColor color);
		static UTF8Char *ToFormatCode(UTF8Char *sbuff, const UTF8Char *dataFormat);
	};
}
#endif
