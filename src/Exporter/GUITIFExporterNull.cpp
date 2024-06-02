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

Bool Exporter::GUITIFExporter::GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("TIFF file"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.tif"));
		return true;
	}
	return false;
}

Bool Exporter::GUITIFExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	Exporter::TIFFExporter exporter;
	return exporter.ExportFile(stm, fileName, pobj, 0);
}

UOSInt Exporter::GUITIFExporter::GetParamCnt()
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

Bool Exporter::GUITIFExporter::GetParamInfo(UOSInt index, ParamInfo *info)
{
	return false;
}

Bool Exporter::GUITIFExporter::SetParamBool(void *param, UOSInt index, Bool val)
{
	return false;
}

Bool Exporter::GUITIFExporter::GetParamBool(void *param, UOSInt index)
{
	return false;
}
