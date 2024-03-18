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

IO::FileExporter::SupportType Exporter::HEIFExporter::IsObjectSupported(NotNullPtr<IO::ParsedObject> pobj)
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

Bool Exporter::HEIFExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	return false;
}

UOSInt Exporter::HEIFExporter::GetParamCnt()
{
	return 1;
}

Optional<IO::FileExporter::ParamData> Exporter::HEIFExporter::CreateParam(NotNullPtr<IO::ParsedObject> pobj)
{
	Int32 *val = MemAlloc(Int32, 1);
	*val = 100;
	return (ParamData*)val;
}

void Exporter::HEIFExporter::DeleteParam(Optional<ParamData> param)
{
	NotNullPtr<ParamData> para;
	if (param.SetTo(para))
	{
		MemFree(para.Ptr());
	}
}

Bool Exporter::HEIFExporter::GetParamInfo(UOSInt index, NotNullPtr<ParamInfo> info)
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

Bool Exporter::HEIFExporter::SetParamInt32(Optional<ParamData> param, UOSInt index, Int32 val)
{
	NotNullPtr<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		if (val >= 0 && val <= 100)
		{
			*(Int32*)para.Ptr() = val;
			return true;
		}
		return false;
	}
	return false;
}

Int32 Exporter::HEIFExporter::GetParamInt32(Optional<ParamData> param, UOSInt index)
{
	NotNullPtr<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		return *(Int32*)para.Ptr();
	}
	return 0;
}
