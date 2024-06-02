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

Bool Exporter::GUIJPGExporter::GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("JPEG file (GDK)"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.jpg"));
		return true;
	}
	return false;
}

Bool Exporter::GUIJPGExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
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
	NN<ParamData> para;

	if (param.SetTo(para))
	{
		Text::StrInt32(cbuff, *(Int32*)para.Ptr());
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
		Optional<Media::RasterImage> srcImg = 0;
		Media::ImageList *imgList;
		if (pobj->GetParserType() == IO::ParserType::ImageList)
		{
			imgList = (Media::ImageList*)pobj.Ptr();
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

Optional<IO::FileExporter::ParamData> Exporter::GUIJPGExporter::CreateParam(NN<IO::ParsedObject> pobj)
{
	Int32 *val = MemAlloc(Int32, 1);
	*val = 100;
	return (ParamData*)val;
}

void Exporter::GUIJPGExporter::DeleteParam(Optional<ParamData> param)
{
	NN<ParamData> para;
	if (param.SetTo(para))
	{
		MemFree(para.Ptr());
	}
}

Bool Exporter::GUIJPGExporter::GetParamInfo(UOSInt index, NN<ParamInfo> info)
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

Bool Exporter::GUIJPGExporter::SetParamInt32(Optional<ParamData> param, UOSInt index, Int32 val)
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

Int32 Exporter::GUIJPGExporter::GetParamInt32(Optional<ParamData> param, UOSInt index)
{
	NN<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		return *(Int32*)para.Ptr();
	}
	return 0;
}
