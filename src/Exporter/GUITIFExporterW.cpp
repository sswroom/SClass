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

Bool Exporter::GUITIFExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, Optional<ParamData> param)
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
	NotNullPtr<ParamData> para;
	if (param.SetTo(para))
	{
		compressed = *(Bool*)para.Ptr();
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

Optional<IO::FileExporter::ParamData> Exporter::GUITIFExporter::CreateParam(NotNullPtr<IO::ParsedObject> pobj)
{
	Bool *val = MemAlloc(Bool, 1);
	*val = false;
	return (ParamData*)val;
}

void Exporter::GUITIFExporter::DeleteParam(Optional<ParamData> param)
{
	NotNullPtr<ParamData> para;
	if (param.SetTo(para))
	{
		MemFree(para.Ptr());
	}
}

Bool Exporter::GUITIFExporter::GetParamInfo(UOSInt index, NotNullPtr<ParamInfo> info)
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

Bool Exporter::GUITIFExporter::SetParamBool(Optional<ParamData> param, UOSInt index, Bool val)
{
	NotNullPtr<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		*(Bool*)para.Ptr() = val;
		return true;
	}
	return false;
}

Bool Exporter::GUITIFExporter::GetParamBool(Optional<ParamData> param, UOSInt index)
{
	NotNullPtr<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		return *(Bool*)para.Ptr();
	}
	return false;
}
