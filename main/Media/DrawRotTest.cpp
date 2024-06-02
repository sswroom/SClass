#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/FileStream.h"
#include "Media/DrawEngineFactory.h"
#include "Text/MyString.h"

void GenerateAngle(NN<Media::DrawEngine> deng, UOSInt angleDegree)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUOSInt(sbuff, angleDegree);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));
	NN<Media::DrawImage> dimg;
	if (deng->CreateImage32(Math::Size2D<UOSInt>(160, 160), Media::AlphaType::AT_NO_ALPHA).SetTo(dimg))
	{
		NN<Media::DrawFont> f = dimg->NewFontPt(CSTR("Arial"), 9, Media::DrawEngine::DFS_NORMAL, 0);
		NN<Media::DrawBrush> b = dimg->NewBrushARGB(0xffffffff);
		dimg->DrawStringRot(Math::Coord2DDbl(80, 80), CSTRP(sbuff, sptr), f, b, UOSInt2Double(angleDegree));
		dimg->DelBrush(b);
		dimg->DelFont(f);
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		dimg->SavePng(fs);
		deng->DeleteImage(dimg);
	}
}

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	NN<Media::DrawEngine> deng = Media::DrawEngineFactory::CreateDrawEngine();
	GenerateAngle(deng, 0);
	GenerateAngle(deng, 45);
	GenerateAngle(deng, 90);
	GenerateAngle(deng, 135);
	GenerateAngle(deng, 180);
	GenerateAngle(deng, 225);
	GenerateAngle(deng, 270);
	GenerateAngle(deng, 315);
	deng.Delete();
	return 0;
}
