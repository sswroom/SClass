#ifndef _SM_EXPORTER_DOCHTMLEXPORTER
#define _SM_EXPORTER_DOCHTMLEXPORTER
#include "IO/FileExporter.h"
#include "IO/Writer.h"
#include "Text/Doc/DocItem.h"

namespace Exporter
{
	class DocHTMLExporter : public IO::FileExporter
	{
	private:
		UInt32 codePage;

	public:
		DocHTMLExporter();
		virtual ~DocHTMLExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);

	private:
		static void WriteColor(IO::Writer *writer, UInt32 color);
		static void WriteItems(IO::Writer *writer, Data::ReadingList<Text::Doc::DocItem *> *items, const UTF8Char *parentNodeName, UOSInt nameLen);
	};
}
#endif
