#include "Stdafx.h"
#include "Exporter/GUIJPGExporter.h"

Exporter::GUIJPGExporter::GUIJPGExporter() : Exporter::GUIExporter()
{
}

Exporter::GUIJPGExporter::~GUIJPGExporter()
{
}

Int32 Exporter::GUIJPGExporter::GetName()
{
	return *(Int32*)"GPJP";
}

Bool Exporter::GUIJPGExporter::GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"JPEG file");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.jpg");
		return true;
	}
	return false;
}

Bool Exporter::GUIJPGExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	return false;
}

OSInt Exporter::GUIJPGExporter::GetParamCnt()
{
	return 1;
}

void *Exporter::GUIJPGExporter::CreateParam(IO::ParsedObject *pobj)
{
	Int32 *val = MemAlloc(Int32, 1);
	*val = 100;
	return val;
}

void Exporter::GUIJPGExporter::DeleteParam(void *param)
{
	MemFree(param);
}

Bool Exporter::GUIJPGExporter::GetParamInfo(OSInt index, ParamInfo *info)
{
	if (index == 0)
	{
		info->name = (const UTF8Char*)"Quality";
		info->paramType = IO::FileExporter::PT_INT32;
		info->allowNull = false;
		return true;
	}
	return false;
}

Bool Exporter::GUIJPGExporter::SetParamInt32(void *param, OSInt index, Int32 val)
{
	if (index == 0)
	{
		if (val >= 0 && val <= 100)
		{
			*(Int32*)param = val;
			return true;
		}
		return false;
	}
	return false;
}

Int32 Exporter::GUIJPGExporter::GetParamInt32(void *param, OSInt index)
{
	if (index == 0)
	{
		return *(Int32*)param;
	}
	return 0;
}
