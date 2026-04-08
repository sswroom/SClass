#include "Stdafx.h"
#include "Core/Core.h"
#include "Exporter/PDFExporter.h"
#include "IO/FileStream.h"
#include "Math/Unit/Distance.h"
#include "Media/DrawEngineFactory.h"
#include "Media/ImageList.h"
#include "Media/PaperSize.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<Media::DrawEngine> deng;
	NN<Media::ImageList> vdoc;
	NN<Media::VectorGraph> g;
	Exporter::PDFExporter exporter;
	NN<IO::FileStream> fs;
	Text::CStringNN fileName;
	Media::PaperSize psize(Media::PaperSize::PT_A4);
	deng = Media::DrawEngineFactory::CreateDrawEngine();
	NEW_CLASSNN(vdoc, Media::ImageList(CSTR("Test")));
	g = vdoc->AddGraph(0, deng, psize.GetWidthMM(), psize.GetHeightMM(), Math::Unit::Distance::DU_MILLIMETER);
	g = vdoc->AddGraph(0, deng, psize.GetHeightMM(), psize.GetWidthMM(), Math::Unit::Distance::DU_MILLIMETER);

	fileName = CSTR("/home/sswroom/Temp/Test.pdf");
	NEW_CLASSNN(fs, IO::FileStream(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	exporter.ExportFile(fs, fileName, vdoc, nullptr);
	fs.Delete();

	vdoc.Delete();
	deng.Delete();
	return 0;
}
