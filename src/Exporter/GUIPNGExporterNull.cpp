#include "Stdafx.h"
#include "Exporter/GUIPNGExporter.h"

Exporter::GUIPNGExporter::GUIPNGExporter() : Exporter::GUIExporter()
{
}

Exporter::GUIPNGExporter::~GUIPNGExporter()
{
}

Int32 Exporter::GUIPNGExporter::GetName()
{
	return *(Int32*)"GPPN";
}

Bool Exporter::GUIPNGExporter::GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"PNG file");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.png");
		return true;
	}
	return false;
}

Bool Exporter::GUIPNGExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	return false;
}
