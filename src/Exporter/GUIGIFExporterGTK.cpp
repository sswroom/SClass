#include "Stdafx.h"
#include "Text/MyString.h"
#include "Exporter/GUIGIFExporter.h"

Exporter::GUIGIFExporter::GUIGIFExporter() : Exporter::GUIExporter()
{
}

Exporter::GUIGIFExporter::~GUIGIFExporter()
{
}

Int32 Exporter::GUIGIFExporter::GetName()
{
	return *(Int32*)"GPGI";
}

Bool Exporter::GUIGIFExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"GIF file (GDK)");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.gif");
		return true;
	}
	return false;
}

Bool Exporter::GUIGIFExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	return false;
}
