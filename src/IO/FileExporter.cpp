#include "Stdafx.h"
#include "IO/FileExporter.h"
#include "IO/FileStream.h"

Bool IO::FileExporter::ExportNewFile(const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	Bool succ = this->ExportFile(fs, fileName, pobj, param);
	DEL_CLASS(fs);
	return succ;
}

void IO::FileExporter::SetCodePage(UInt32 codePage)
{
}

void IO::FileExporter::SetEncFactory(Text::EncodingFactory *encFact)
{
}

UOSInt IO::FileExporter::GetParamCnt()
{
	return 0;
}

void *IO::FileExporter::CreateParam(IO::ParsedObject *pobj)
{
	return 0;
}

void IO::FileExporter::DeleteParam(void *param)
{
}

Bool IO::FileExporter::GetParamInfo(UOSInt index, ParamInfo *info)
{
	return false;
}

Bool IO::FileExporter::SetParamStr(void *param, UOSInt index, const UTF8Char *val)
{
	return false;
}

Bool IO::FileExporter::SetParamInt32(void *param, UOSInt index, Int32 val)
{
	return false;
}

Bool IO::FileExporter::SetParamSel(void *param, UOSInt index, UOSInt selCol)
{
	return false;
}

Bool IO::FileExporter::SetParamBool(void *param, UOSInt index, Bool selCol)
{
	return false;
}

UTF8Char *IO::FileExporter::GetParamStr(void *param, UOSInt index, UTF8Char *buff)
{
	return 0;
}

Int32 IO::FileExporter::GetParamInt32(void *param, UOSInt index)
{
	return 0;
}

Int32 IO::FileExporter::GetParamSel(void *param, UOSInt index)
{
	return 0;
}

UTF8Char *IO::FileExporter::GetParamSelItems(void *param, UOSInt index, UOSInt itemIndex, UTF8Char *buff)
{
	return 0;
}

Bool IO::FileExporter::GetParamBool(void *param, UOSInt index)
{
	return false;
}
