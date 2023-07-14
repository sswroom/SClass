#include "Stdafx.h"
#include "Text/MyString.h"
#include "Exporter/GUIGIFExporter.h"

#if defined(WIN32_LEAN_AND_MEAN)
#undef WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include "Win32/COMStream.h"
#ifndef _WIN32_WCE
#include <gdiplus.h>
#endif

Exporter::GUIGIFExporter::GUIGIFExporter() : Exporter::GUIExporter()
{
}

Exporter::GUIGIFExporter::~GUIGIFExporter()
{
}

Int32 Exporter::GUIGIFExporter::GetName()
{
	return *(Int32*)"GPGI";
}

Bool Exporter::GUIGIFExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("GIF file (GDI+)"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.gif"));
		return true;
	}
	return false;
}

Bool Exporter::GUIGIFExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CString fileName, IO::ParsedObject *pobj, void *param)
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

	if (GetEncoderClsid(L"image/gif", &encoderClsid) < 0)
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

	stat = image->Save(cstm, &encoderClsid, 0);

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
