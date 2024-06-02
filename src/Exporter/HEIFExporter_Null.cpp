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

IO::FileExporter::SupportType Exporter::HEIFExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::HEIFExporter::GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("HEIF container"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.heic"));
		return true;
	}
	return false;
}

Bool Exporter::HEIFExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	return false;
}

UOSInt Exporter::HEIFExporter::GetParamCnt()
{
	return 1;
}

Optional<IO::FileExporter::ParamData> Exporter::HEIFExporter::CreateParam(NN<IO::ParsedObject> pobj)
{
	Int32 *val = MemAlloc(Int32, 1);
	*val = 100;
	return (ParamData*)val;
}

void Exporter::HEIFExporter::DeleteParam(Optional<ParamData> param)
{
	NN<ParamData> para;
	if (param.SetTo(para))
	{
		MemFree(para.Ptr());
	}
}

Bool Exporter::HEIFExporter::GetParamInfo(UOSInt index, NN<ParamInfo> info)
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
	NN<ParamData> para;
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
	NN<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		return *(Int32*)para.Ptr();
	}
	return 0;
}
