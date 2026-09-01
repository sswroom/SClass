#include "Stdafx.h"
#include "Exporter/AVIFExporter.h"

Exporter::AVIFExporter::AVIFExporter()
{
}

Exporter::AVIFExporter::~AVIFExporter()
{
}

Int32 Exporter::AVIFExporter::GetName()
{
	return *(Int32*)"AVIF";
}

IO::FileExporter::SupportType Exporter::AVIFExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::AVIFExporter::GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("AVIF image"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.avif"));
		return true;
	}
	return false;
}

Bool Exporter::AVIFExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	return false;
}

UIntOS Exporter::AVIFExporter::GetParamCnt()
{
	return 1;
}

Optional<IO::FileExporter::ParamData> Exporter::AVIFExporter::CreateParam(NN<IO::ParsedObject> pobj)
{
	UnsafeArray<Int32> val = MemAllocArr(Int32, 1);
	val[0] = 90;
	return (ParamData*)val.Ptr();
}

void Exporter::AVIFExporter::DeleteParam(Optional<ParamData> param)
{
	NN<ParamData> para;
	if (param.SetTo(para))
	{
		MemFree(para.Ptr());
	}
}

Bool Exporter::AVIFExporter::GetParamInfo(UIntOS index, NN<ParamInfo> info)
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

Bool Exporter::AVIFExporter::SetParamInt32(Optional<ParamData> param, UIntOS index, Int32 val)
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

Int32 Exporter::AVIFExporter::GetParamInt32(Optional<ParamData> param, UIntOS index)
{
	NN<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		return *(Int32*)para.Ptr();
	}
	return 0;
}