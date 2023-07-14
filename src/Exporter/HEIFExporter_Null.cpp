#include "Stdafx.h"
#include "Exporter/HEIFExporter.h"

Exporter::HEIFExporter::HEIFExporter()
{
}

Exporter::HEIFExporter::~HEIFExporter()
{
}

Int32 Exporter::HEIFExporter::GetName()
{
	return *(Int32*)"HIEF";
}

IO::FileExporter::SupportType Exporter::HEIFExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::HEIFExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("HEIF container"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.heic"));
		return true;
	}
	return false;
}

Bool Exporter::HEIFExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CString fileName, IO::ParsedObject *pobj, void *param)
{
	return false;
}

UOSInt Exporter::HEIFExporter::GetParamCnt()
{
	return 1;
}

void *Exporter::HEIFExporter::CreateParam(IO::ParsedObject *pobj)
{
	Int32 *val = MemAlloc(Int32, 1);
	*val = 100;
	return val;
}

void Exporter::HEIFExporter::DeleteParam(void *param)
{
	MemFree(param);
}

Bool Exporter::HEIFExporter::GetParamInfo(UOSInt index, ParamInfo *info)
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

Bool Exporter::HEIFExporter::SetParamInt32(void *param, UOSInt index, Int32 val)
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

Int32 Exporter::HEIFExporter::GetParamInt32(void *param, UOSInt index)
{
	if (index == 0)
	{
		return *(Int32*)param;
	}
	return 0;
}
