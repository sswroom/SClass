#include "Stdafx.h"
#include "Text/MyString.h"
#include "Exporter/GUITIFExporter.h"
#include <gdk-pixbuf/gdk-pixbuf.h>

Exporter::GUITIFExporter::GUITIFExporter() : Exporter::GUIExporter()
{
}

Exporter::GUITIFExporter::~GUITIFExporter()
{
}

Int32 Exporter::GUITIFExporter::GetName()
{
	return *(Int32*)"GPTI";
}

Bool Exporter::GUITIFExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("TIFF file (GDK)"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.tif"));
		return true;
	}
	return false;
}

Bool Exporter::GUITIFExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CString fileName, IO::ParsedObject *pobj, void *param)
{
	UInt8 *tmpBuff;
	GdkPixbuf *image = (GdkPixbuf*)ToImage(pobj, &tmpBuff);
	if (image == 0)
	{
		return false;
	}

	gchar *buff = 0;
	gsize buffSize;

	gdk_pixbuf_save_to_buffer(image, &buff, &buffSize, "tiff", 0, (void*)0);
	g_object_unref(image);
	if (tmpBuff)
	{
		MemFreeA(tmpBuff);
	}

	if (buff)
	{
		stm->Write((UInt8*)buff, buffSize);
		g_free(buff);
		return true;
	}
	return false;
}

UOSInt Exporter::GUITIFExporter::GetParamCnt()
{
	return 0;
//	return 1;
}

void *Exporter::GUITIFExporter::CreateParam(IO::ParsedObject *pobj)
{
	return 0;
/*	Bool *val = MemAlloc(Bool, 1);
	*val = false;
	return val;*/
}

void Exporter::GUITIFExporter::DeleteParam(void *param)
{
//	MemFree(param);
}

Bool Exporter::GUITIFExporter::GetParamInfo(UOSInt index, ParamInfo *info)
{
/*	if (index == 0)
	{
		info->name = L"Compression";
		info->paramType = IO::FileExporter::ParamType::BOOL;
		info->allowNull = false;
		return true;
	}*/
	return false;
}

Bool Exporter::GUITIFExporter::SetParamBool(void *param, UOSInt index, Bool val)
{
/*	if (index == 0)
	{
		*(Bool*)param = val;
		return true;
	}*/
	return false;
}

Bool Exporter::GUITIFExporter::GetParamBool(void *param, UOSInt index)
{
/*	if (index == 0)
	{
		return *(Bool*)param;
	}*/
	return false;
}
