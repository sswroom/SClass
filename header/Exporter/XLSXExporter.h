#ifndef _SM_EXPORTER_XLSXEXPORTER
#define _SM_EXPORTER_XLSXEXPORTER
#include "Data/ArrayListObj.hpp"
#include "Data/DateTime.h"
#include "Data/StringMapNative.hpp"
#include "IO/FileExporter.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/SpreadSheet/OfficeChartAxis.h"
#include "Text/SpreadSheet/OfficeChartSeries.h"
#include "Text/SpreadSheet/OfficeShapeProp.h"
#include "Text/SpreadSheet/Workbook.h"

namespace Exporter
{
	class XLSXExporter : public IO::FileExporter
	{
	private:
		struct LinkInfo
		{
			UIntOS row;
			UIntOS col;
			NN<Text::SpreadSheet::Worksheet::CellData> cell;
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
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);

		static void AppendFill(NN<Text::StringBuilderUTF8> sb, Optional<Text::SpreadSheet::OfficeFill> fill);
		static void AppendLineStyle(NN<Text::StringBuilderUTF8> sb, Optional<Text::SpreadSheet::OfficeLineStyle> lineStyle);
		static void AppendTitle(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> title);
		static void AppendShapeProp(NN<Text::StringBuilderUTF8> sb, Optional<Text::SpreadSheet::OfficeShapeProp> shapeProp);
		static void AppendAxis(NN<Text::StringBuilderUTF8> sb, Optional<Text::SpreadSheet::OfficeChartAxis> axis, UIntOS index);
		static void AppendSeries(NN<Text::StringBuilderUTF8> sb, NN<Text::SpreadSheet::OfficeChartSeries> series, UIntOS index);
		static void AppendBorder(NN<Text::StringBuilderUTF8> sb, Text::SpreadSheet::CellStyle::BorderStyle border, Text::CStringNN name);
		static void AppendXF(NN<Text::StringBuilderUTF8> sb, NN<Text::SpreadSheet::CellStyle> style, NN<Data::ArrayListNN<BorderInfo>> borders, NN<Text::SpreadSheet::Workbook> workbook, NN<Data::StringMapNative<UIntOS>> numFmtMap);

		static Text::CStringNN PresetColorCode(Text::SpreadSheet::PresetColor color);
		static UnsafeArray<UTF8Char> ToFormatCode(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> dataFormat);
	};
}
#endif
