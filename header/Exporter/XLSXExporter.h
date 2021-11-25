#ifndef _SM_EXPORTER_XLSXEXPORTER
#define _SM_EXPORTER_XLSXEXPORTER
#include "Data/DateTime.h"
#include "IO/FileExporter.h"
#include "Text/StringBuilderUTF.h"
#include "Text/SpreadSheet/OfficeLineStyle.h"

namespace Exporter
{
	class XLSXExporter : public IO::FileExporter
	{
	public:
		XLSXExporter();
		virtual ~XLSXExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param);

		static void AppendFill(Text::StringBuilderUTF *sb, Text::SpreadSheet::OfficeFill *fill);
		static void AppendLineStyle(Text::StringBuilderUTF *sb, Text::SpreadSheet::OfficeLineStyle *lineStyle);
		static void AppendTitle(Text::StringBuilderUTF *sb, const UTF8Char *title);

		static const Char *PresetColorCode(Text::SpreadSheet::PresetColor color);
		static UTF8Char *ToFormatCode(UTF8Char *sbuff, const UTF8Char *dataFormat);
	};
}
#endif
