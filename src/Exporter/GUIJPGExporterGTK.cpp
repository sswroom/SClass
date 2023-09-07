#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Exporter/GUIJPGExporter.h"
#include "IO/MemoryStream.h"
#include "Media/ImageList.h"
#include "Media/JPEGFile.h"
#include "Text/MyString.h"
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <wchar.h>

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

Bool Exporter::GUIJPGExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("JPEG file (GDK)"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.jpg"));
		return true;
	}
	return false;
}

Bool Exporter::GUIJPGExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, IO::ParsedObject *pobj, void *param)
{
	UInt8 *tmpBuff;
	GdkPixbuf *image = (GdkPixbuf*)ToImage(pobj, &tmpBuff);
	if (image == 0)
	{
		return false;
	}
	Char cbuff[32];
	gchar *buff = 0;
	gsize buffSize;

	if (param)
	{
		Text::StrInt32(cbuff, *(Int32*)param);
		gdk_pixbuf_save_to_buffer(image, &buff, &buffSize, "jpeg", 0, "quality", cbuff, (void*)0);
	}
	else
	{
		gdk_pixbuf_save_to_buffer(image, &buff, &buffSize, "jpeg", 0, (void*)0);
	}
	g_object_unref(image);
	if (tmpBuff)
	{
		MemFreeA(tmpBuff);
	}

	if (buff)
	{
		Media::Image *srcImg = 0;
		Media::ImageList *imgList;
		if (pobj->GetParserType() == IO::ParserType::ImageList)
		{
			imgList = (Media::ImageList*)pobj;
			srcImg = imgList->GetImage(0, 0);
		}
		Media::JPEGFile::WriteJPGBuffer(stm, (const UInt8*)buff, buffSize, srcImg);
		g_free(buff);
		return true;
	}
	else
	{
	}
	return false;
}

UOSInt Exporter::GUIJPGExporter::GetParamCnt()
{
	return 1;
}

void *Exporter::GUIJPGExporter::CreateParam(IO::ParsedObject *pobj)
{
	Int32 *val = MemAlloc(Int32, 1);
	*val = 100;
	return val;
}

void Exporter::GUIJPGExporter::DeleteParam(void *param)
{
	MemFree(param);
}

Bool Exporter::GUIJPGExporter::GetParamInfo(UOSInt index, ParamInfo *info)
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

Bool Exporter::GUIJPGExporter::SetParamInt32(void *param, UOSInt index, Int32 val)
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

Int32 Exporter::GUIJPGExporter::GetParamInt32(void *param, UOSInt index)
{
	if (index == 0)
	{
		return *(Int32*)param;
	}
	return 0;
}
