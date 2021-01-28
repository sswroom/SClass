#include "Stdafx.h"
#include "Text/MyString.h"
#include "Exporter/GUITIFExporter.h"
#include "Exporter/TIFFExporter.h"

Exporter::GUITIFExporter::GUITIFExporter() : Exporter::GUIExporter()
{
}

Exporter::GUITIFExporter::~GUITIFExporter()
{
}

Int32 Exporter::GUITIFExporter::GetName()
{
	return *(Int32*)"GPTI";
}

Bool Exporter::GUITIFExporter::GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"TIFF file");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.tif");
		return true;
	}
	return false;
}

Bool Exporter::GUITIFExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	Exporter::TIFFExporter exporter;
	return exporter.ExportFile(stm, fileName, pobj, 0);
}

OSInt Exporter::GUITIFExporter::GetParamCnt()
{
	return 0;
}

void *Exporter::GUITIFExporter::CreateParam(IO::ParsedObject *pobj)
{
	return 0;
}

void Exporter::GUITIFExporter::DeleteParam(void *param)
{
}

Bool Exporter::GUITIFExporter::GetParamInfo(OSInt index, ParamInfo *info)
{
	return false;
}

Bool Exporter::GUITIFExporter::SetParamBool(void *param, OSInt index, Bool val)
{
	return false;
}

Bool Exporter::GUITIFExporter::GetParamBool(void *param, OSInt index)
{
	return false;
}
