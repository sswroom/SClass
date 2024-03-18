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

IO::FileExporter::SupportType Exporter::WebPExporter::IsObjectSupported(NotNullPtr<IO::ParsedObject> pobj)
{
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::WebPExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	return false;
}

Bool Exporter::WebPExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	return false;
}

UOSInt Exporter::WebPExporter::GetParamCnt()
{
	return 1;
}

Optional<IO::FileExporter::ParamData> Exporter::WebPExporter::CreateParam(NotNullPtr<IO::ParsedObject> pobj)
{
	Int32 *val = MemAlloc(Int32, 1);
	*val = 100;
	return (ParamData*)val;
}

void Exporter::WebPExporter::DeleteParam(Optional<ParamData> param)
{
	NotNullPtr<ParamData> para;
	if (param.SetTo(para))
	{
		MemFree(para.Ptr());
	}
}

Bool Exporter::WebPExporter::GetParamInfo(UOSInt index, NotNullPtr<ParamInfo> info)
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

Bool Exporter::WebPExporter::SetParamInt32(Optional<ParamData> param, UOSInt index, Int32 val)
{
	NotNullPtr<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		if (val <= 100)
		{
			*(Int32*)para.Ptr() = val;
			return true;
		}
		return false;
	}
	return false;
}

Int32 Exporter::WebPExporter::GetParamInt32(Optional<ParamData> param, UOSInt index)
{
	NotNullPtr<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		return *(Int32*)para.Ptr();
	}
	return 0;
}
