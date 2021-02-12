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
		Int32 codePage;

	public:
		DocHTMLExporter();
		virtual ~DocHTMLExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(Int32 codePage);
		virtual Bool ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param);

	private:
		static void WriteColor(IO::Writer *writer, UInt32 color);
		static void WriteItems(IO::Writer *writer, Data::ReadingList<Text::Doc::DocItem *> *items, const UTF8Char *parentNodeName);
	};
}
#endif
