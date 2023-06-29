#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Exporter/GUIJPGExporter.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Text::CString fileName = CSTR("export.jpg");
	Exporter::GUIJPGExporter exporter;
	Media::StaticImage *simg;

	Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
	NEW_CLASS(simg, Media::StaticImage(Math::Size2D<UOSInt>(320, 240), 0, 32, Media::PF_B8G8R8A8, 320 * 240 * 4, &color, Media::ColorProfile::YUVT_BT601, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	Media::ImageList imgList(CSTR("Temp"));
	imgList.AddImage(simg, 0);
	{
		IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		exporter.ExportFile(&fs, fileName, &imgList, 0);
	}
	return 0;
}
