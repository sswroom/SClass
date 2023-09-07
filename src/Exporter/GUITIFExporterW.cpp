#include "Stdafx.h"
#include "Text/MyString.h"
#include "Exporter/GUITIFExporter.h"

#if defined(WIN32_LEAN_AND_MEAN)
#undef WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include "Win32/COMStream.h"
#ifndef _WIN32_WCE
#include <gdiplus.h>
#endif

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
		Text::StrConcatC(nameBuff, UTF8STRC("TIFF file (GDI+)"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.tif"));
		return true;
	}
	return false;
}

Bool Exporter::GUITIFExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, IO::ParsedObject *pobj, void *param)
{
#ifdef _WIN32_WCE
	return false;
#else
	CLSID   encoderClsid;
	Gdiplus::Status  stat;
	Gdiplus::Image*   image;
	UInt8 *relBuff;

	image = (Gdiplus::Image*)ToImage(pobj, &relBuff);
	if (image == 0)
	{
		return false;
	}

	if (GetEncoderClsid(L"image/tiff", &encoderClsid) < 0)
	{
		DEL_CLASS(image);
		if (relBuff)
		{
			MemFreeA(relBuff);
		}
		return false;
	}

	Win32::COMStream *cstm;
	NEW_CLASS(cstm, Win32::COMStream(stm));
	Bool compressed = false;

	if (param)
	{
		compressed = *(Bool*)param;
	}
	Int32 compress = compressed?Gdiplus::EncoderValueCompressionLZW:Gdiplus::EncoderValueCompressionNone;
	Gdiplus::EncoderParameters params;
	params.Count = 1;
	params.Parameter[0].Guid = Gdiplus::EncoderCompression;
	params.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
	params.Parameter[0].NumberOfValues = 1;
	params.Parameter[0].Value = &compress;
	stat = image->Save(cstm, &encoderClsid, &params);

	DEL_CLASS(cstm);
	DEL_CLASS(image);
	if (relBuff)
	{
		MemFreeA(relBuff);
	}

	if(stat == Gdiplus::Ok)
		return true;
	else
		return false;
#endif
}

UOSInt Exporter::GUITIFExporter::GetParamCnt()
{
	return 1;
}

void *Exporter::GUITIFExporter::CreateParam(IO::ParsedObject *pobj)
{
	Bool *val = MemAlloc(Bool, 1);
	*val = false;
	return val;
}

void Exporter::GUITIFExporter::DeleteParam(void *param)
{
	MemFree(param);
}

Bool Exporter::GUITIFExporter::GetParamInfo(UOSInt index, ParamInfo *info)
{
	if (index == 0)
	{
		info->name = CSTR("Compression");
		info->paramType = IO::FileExporter::ParamType::BOOL;
		info->allowNull = false;
		return true;
	}
	return false;
}

Bool Exporter::GUITIFExporter::SetParamBool(void *param, UOSInt index, Bool val)
{
	if (index == 0)
	{
		*(Bool*)param = val;
		return true;
	}
	return false;
}

Bool Exporter::GUITIFExporter::GetParamBool(void *param, UOSInt index)
{
	if (index == 0)
	{
		return *(Bool*)param;
	}
	return false;
}
