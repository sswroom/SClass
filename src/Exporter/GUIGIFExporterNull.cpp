#include "Stdafx.h"
#include "Exporter/GIFExporter.h"
#include "Exporter/GUIGIFExporter.h"
#include "Text/MyString.h"

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

Bool Exporter::GUIGIFExporter::GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("GIF file"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.gif"));
		return true;
	}
	return false;
}

Bool Exporter::GUIGIFExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	Exporter::GIFExporter exporter;
	return exporter.ExportFile(stm, fileName, pobj, 0);
}
