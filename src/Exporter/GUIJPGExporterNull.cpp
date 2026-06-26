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

Bool Exporter::GUIJPGExporter::GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("JPEG file"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.jpg"));
		return true;
	}
	return false;
}

Bool Exporter::GUIJPGExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	return false;
}

UIntOS Exporter::GUIJPGExporter::GetParamCnt()
{
	return 1;
}

Optional<IO::FileExporter::ParamData> Exporter::GUIJPGExporter::CreateParam(NN<IO::ParsedObject> pobj)
{
	UnsafeArray<Int32> val = MemAllocArr(Int32, 1);
	val[0] = 100;
	return (ParamData*)val.Ptr();
}

void Exporter::GUIJPGExporter::DeleteParam(Optional<ParamData> param)
{
	NN<ParamData> para;
	if (param.SetTo(para))
	{
		MemFree(para.Ptr());
	}
}

Bool Exporter::GUIJPGExporter::GetParamInfo(UIntOS index, NN<ParamInfo> info)
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

Bool Exporter::GUIJPGExporter::SetParamInt32(Optional<ParamData> param, UIntOS index, Int32 val)
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

Int32 Exporter::GUIJPGExporter::GetParamInt32(Optional<ParamData> param, UIntOS index)
{
	NN<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		return *(Int32*)para.Ptr();
	}
	return 0;
}
