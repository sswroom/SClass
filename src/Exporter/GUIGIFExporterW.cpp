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

Bool Exporter::GUIGIFExporter::GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("GIF file (GDI+)"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.gif"));
		return true;
	}
	return false;
}

Bool Exporter::GUIGIFExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
#ifdef _WIN32_WCE
	return false;
#else
	CLSID   encoderClsid;
	Gdiplus::Status  stat;
	NN<Gdiplus::Image>   image;
	UnsafeArrayOpt<UInt8> relBuff;
	UnsafeArray<UInt8> nnrelBuff;

	if (!ToImage(pobj, false, relBuff).GetOpt<Gdiplus::Image>().SetTo(image))
	{
		return false;
	}

	if (GetEncoderClsid(L"image/gif", &encoderClsid) < 0)
	{
		image.Delete();
		if (relBuff.SetTo(nnrelBuff))
		{
			MemFreeAArr(relBuff);
		}
		return false;
	}

	NN<Win32::COMStream> cstm;
	NEW_CLASSNN(cstm, Win32::COMStream(stm));

	stat = image->Save(cstm.Ptr(), &encoderClsid, 0);

	cstm.Delete();
	image.Delete();
	if (relBuff.SetTo(nnrelBuff))
	{
		MemFreeAArr(nnrelBuff);
	}

	if(stat == Gdiplus::Ok)
		return true;
	else
		return false;
#endif
}
