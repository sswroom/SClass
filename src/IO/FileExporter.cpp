#include "Stdafx.h"
#include "IO/FileExporter.h"
#include "IO/FileStream.h"

Bool IO::FileExporter::ExportNewFile(Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return false;
	}
	Bool succ = this->ExportFile(fs, fileName, pobj, param);
	return succ;
}

void IO::FileExporter::SetCodePage(UInt32 codePage)
{
}

void IO::FileExporter::SetEncFactory(Optional<Text::EncodingFactory> encFact)
{
}

UIntOS IO::FileExporter::GetParamCnt()
{
	return 0;
}

Optional<IO::FileExporter::ParamData> IO::FileExporter::CreateParam(NN<IO::ParsedObject> pobj)
{
	return nullptr;
}

void IO::FileExporter::DeleteParam(Optional<ParamData> param)
{
}

Bool IO::FileExporter::GetParamInfo(UIntOS index, NN<ParamInfo> info)
{
	return false;
}

Bool IO::FileExporter::SetParamStr(Optional<ParamData> param, UIntOS index, UnsafeArrayOpt<const UTF8Char> val)
{
	return false;
}

Bool IO::FileExporter::SetParamInt32(Optional<ParamData> param, UIntOS index, Int32 val)
{
	return false;
}

Bool IO::FileExporter::SetParamSel(Optional<ParamData> param, UIntOS index, UIntOS selCol)
{
	return false;
}

Bool IO::FileExporter::SetParamBool(Optional<ParamData> param, UIntOS index, Bool selCol)
{
	return false;
}

UnsafeArrayOpt<UTF8Char> IO::FileExporter::GetParamStr(Optional<ParamData> param, UIntOS index, UnsafeArray<UTF8Char> buff)
{
	return nullptr;
}

Int32 IO::FileExporter::GetParamInt32(Optional<ParamData> param, UIntOS index)
{
	return 0;
}

Int32 IO::FileExporter::GetParamSel(Optional<ParamData> param, UIntOS index)
{
	return 0;
}

UnsafeArrayOpt<UTF8Char> IO::FileExporter::GetParamSelItems(Optional<ParamData> param, UIntOS index, UIntOS itemIndex, UnsafeArray<UTF8Char> buff)
{
	return nullptr;
}

Bool IO::FileExporter::GetParamBool(Optional<ParamData> param, UIntOS index)
{
	return false;
}
