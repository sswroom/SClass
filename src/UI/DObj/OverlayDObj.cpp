#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "Math/Math_C.h"
#include "UI/DObj/OverlayDObj.h"

UI::DObj::OverlayDObj::OverlayDObj(NN<Media::DrawEngine> deng, Optional<Media::DrawImage> bmp, Math::Coord2D<OSInt> tl) : DirectObject(tl)
{
	this->deng = deng;
	this->noRelease = true;
	this->bmp = bmp;
	this->imgList = nullptr;
	this->frameDelay = 500;
	this->clk = 0;
}

UI::DObj::OverlayDObj::OverlayDObj(NN<Media::DrawEngine> deng, Text::CString fileName, Math::Coord2D<OSInt> tl, NN<Parser::ParserList> parsers) : DirectObject(tl)
{
	this->deng = deng;
	this->noRelease = false;
	this->bmp = nullptr;
	this->clk = 0;
	if (fileName.leng == 0)
	{
		this->imgList = nullptr;
	}
	else
	{
		IO::StmData::FileData fd(fileName.OrEmpty(), false);
		this->imgList = Optional<Media::ImageList>::ConvertFrom(parsers->ParseFileType(fd, IO::ParserType::ImageList));
		this->frameDelay = 500;
		this->startTime = 0;
		this->lastFrameNum = -1;
		NEW_CLASS(this->clk, Manage::HiResClock());
	}
}

UI::DObj::OverlayDObj::~OverlayDObj()
{
	NN<Media::DrawImage> img;
	if (this->noRelease)
	{
	}
	else if (this->bmp.SetTo(img))
	{
		this->deng->DeleteImage(img);
		this->bmp = nullptr;
	}
	else if (this->imgList.NotNull())
	{
		this->imgList.Delete();
	}
	SDEL_CLASS(this->clk);
}

Bool UI::DObj::OverlayDObj::IsChanged()
{
	NN<Media::ImageList> imgList;
	if (this->bmp.NotNull())
	{
		return false;
	}
	else if (this->imgList.SetTo(imgList))
	{
		if (imgList->GetCount() <= 1)
			return false;
		Double t = clk->GetTimeDiff();
		OSInt i = Double2Int32((t - this->startTime) * 1000 / OSInt2Double(this->frameDelay));
		while (i >= (OSInt)imgList->GetCount())
		{
			i -= (OSInt)imgList->GetCount();
			this->startTime += OSInt2Double(this->frameDelay * (OSInt)imgList->GetCount()) * 0.001;
		}
		return i != this->lastFrameNum;
	}
	return false;
}

Bool UI::DObj::OverlayDObj::DoEvents()
{
	return false;
}

void UI::DObj::OverlayDObj::DrawObject(NN<Media::DrawImage> dimg)
{
	NN<Media::DrawImage> bmp;
	NN<Media::ImageList> imgList;
	if (this->bmp.SetTo(bmp))
	{
		Math::Coord2DDbl tl = GetCurrPos().ToDouble();
		dimg->DrawImagePt(bmp, tl);
	}
	else if (this->imgList.SetTo(imgList))
	{
		UOSInt frameNum;
		if (imgList->GetCount() <= 1)
		{
			frameNum = 0;
		}
		else
		{
			Double t = clk->GetTimeDiff();
			frameNum = (UInt32)Double2Int32((t - this->startTime) * 1000 / OSInt2Double(this->frameDelay));
			while (frameNum >= imgList->GetCount())
			{
				frameNum -= imgList->GetCount();
				this->startTime += OSInt2Double(this->frameDelay * (OSInt)imgList->GetCount()) * 0.001;
			}
		}
		imgList->ToStaticImage(frameNum);
		NN<Media::StaticImage> img;
		if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(frameNum, 0)).SetTo(img))
		{
			Math::Coord2DDbl tl = GetCurrPos().ToDouble();
			dimg->DrawImagePt2(img, tl);
		}
	}
}

Bool UI::DObj::OverlayDObj::IsObject(Math::Coord2D<OSInt> scnPos)
{
	return false;
}

/*System::Windows::Forms::Cursor ^UI::DObj::OverlayDObj::GetCursor()
{
	return System::Windows::Forms::Cursors::Arrow;
}*/

void UI::DObj::OverlayDObj::OnMouseDown()
{
}

void UI::DObj::OverlayDObj::OnMouseUp()
{
}

void UI::DObj::OverlayDObj::OnMouseClick()
{
}

void UI::DObj::OverlayDObj::SetFrameDelay(OSInt frameDelay)
{
	if (frameDelay > 0)
	{
		this->frameDelay = frameDelay;
	}
}
