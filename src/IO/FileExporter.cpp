#include "Stdafx.h"
#include "IO/FileExporter.h"

void IO::FileExporter::SetCodePage(Int32 codePage)
{
}

void IO::FileExporter::SetEncFactory(Text::EncodingFactory *encFact)
{
}

OSInt IO::FileExporter::GetParamCnt()
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

Bool IO::FileExporter::GetParamInfo(OSInt index, ParamInfo *info)
{
	return false;
}

Bool IO::FileExporter::SetParamStr(void *param, OSInt index, const UTF8Char *val)
{
	return false;
}

Bool IO::FileExporter::SetParamInt32(void *param, OSInt index, Int32 val)
{
	return false;
}

Bool IO::FileExporter::SetParamSel(void *param, OSInt index, OSInt selCol)
{
	return false;
}

Bool IO::FileExporter::SetParamBool(void *param, OSInt index, Bool selCol)
{
	return false;
}

UTF8Char *IO::FileExporter::GetParamStr(void *param, OSInt index, UTF8Char *buff)
{
	return 0;
}

Int32 IO::FileExporter::GetParamInt32(void *param, OSInt index)
{
	return 0;
}

Int32 IO::FileExporter::GetParamSel(void *param, OSInt index)
{
	return 0;
}

UTF8Char *IO::FileExporter::GetParamSelItems(void *param, OSInt index, OSInt itemIndex, UTF8Char *buff)
{
	return 0;
}

Bool IO::FileExporter::GetParamBool(void *param, OSInt index)
{
	return false;
}
