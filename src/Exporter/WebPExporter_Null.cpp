#include "Stdafx.h"
#include "Exporter/WebPExporter.h"
#include "Text/MyString.h"

Exporter::WebPExporter::WebPExporter()
{
}

Exporter::WebPExporter::~WebPExporter()
{
}

Int32 Exporter::WebPExporter::GetName()
{
	return *(Int32*)"WEBP";
}

IO::FileExporter::SupportType Exporter::WebPExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::WebPExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	return false;
}

Bool Exporter::WebPExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CString fileName, IO::ParsedObject *pobj, void *param)
{
	return false;
}

UOSInt Exporter::WebPExporter::GetParamCnt()
{
	return 1;
}

void *Exporter::WebPExporter::CreateParam(IO::ParsedObject *pobj)
{
	Int32 *val = MemAlloc(Int32, 1);
	*val = 100;
	return val;
}

void Exporter::WebPExporter::DeleteParam(void *param)
{
	MemFree(param);
}

Bool Exporter::WebPExporter::GetParamInfo(UOSInt index, ParamInfo *info)
{
	if (index == 0)
	{
		info->name = CSTR("Quality");
		info->paramType = IO::FileExporter::ParamType::INT32;
		info->allowNull = false;
		return true;
	}
	return false;
}

Bool Exporter::WebPExporter::SetParamInt32(void *param, UOSInt index, Int32 val)
{
	if (index == 0)
	{
		if (val <= 100)
		{
			*(Int32*)param = val;
			return true;
		}
		return false;
	}
	return false;
}

Int32 Exporter::WebPExporter::GetParamInt32(void *param, UOSInt index)
{
	if (index == 0)
	{
		return *(Int32*)param;
	}
	return 0;
}
