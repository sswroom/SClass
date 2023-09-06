#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "UI/DObj/OverlayDObj.h"

UI::DObj::OverlayDObj::OverlayDObj(NotNullPtr<Media::DrawEngine> deng, Media::DrawImage *bmp, Math::Coord2D<OSInt> tl) : DirectObject(tl)
{
	this->deng = deng;
	this->noRelease = true;
	this->bmp = bmp;
	this->imgList = 0;
	this->frameDelay = 500;
	this->clk = 0;
}

UI::DObj::OverlayDObj::OverlayDObj(NotNullPtr<Media::DrawEngine> deng, Text::CString fileName, Math::Coord2D<OSInt> tl, Parser::ParserList *parsers) : DirectObject(tl)
{
	this->deng = deng;
	this->noRelease = false;
	this->bmp = 0;
	this->clk = 0;
	if (fileName.leng == 0)
	{
		this->imgList = 0;
	}
	else
	{
		IO::StmData::FileData fd(fileName.OrEmpty(), false);
		this->imgList = (Media::ImageList*)parsers->ParseFileType(fd, IO::ParserType::ImageList);
		this->frameDelay = 500;
		this->startTime = 0;
		this->lastFrameNum = -1;
		NEW_CLASS(this->clk, Manage::HiResClock());
	}
}

UI::DObj::OverlayDObj::~OverlayDObj()
{
	NotNullPtr<Media::DrawImage> img;
	if (this->noRelease)
	{
	}
	else if (img.Set(this->bmp))
	{
		this->deng->DeleteImage(img);
		this->bmp = 0;
	}
	else if (this->imgList)
	{
		DEL_CLASS(this->imgList);
		this->imgList = 0;
	}
	SDEL_CLASS(this->clk);
}

Bool UI::DObj::OverlayDObj::IsChanged()
{
	if (this->bmp)
	{
		return false;
	}
	else if (this->imgList)
	{
		if (this->imgList->GetCount() <= 1)
			return false;
		Double t = clk->GetTimeDiff();
		OSInt i = Double2Int32((t - this->startTime) * 1000 / OSInt2Double(this->frameDelay));
		while (i >= (OSInt)this->imgList->GetCount())
		{
			i -= (OSInt)this->imgList->GetCount();
			this->startTime += OSInt2Double(this->frameDelay * (OSInt)this->imgList->GetCount()) * 0.001;
		}
		return i != this->lastFrameNum;
	}
	return false;
}

Bool UI::DObj::OverlayDObj::DoEvents()
{
	return false;
}

void UI::DObj::OverlayDObj::DrawObject(NotNullPtr<Media::DrawImage> dimg)
{
	NotNullPtr<Media::DrawImage> bmp;
	if (bmp.Set(this->bmp))
	{
		Math::Coord2DDbl tl = GetCurrPos().ToDouble();
		dimg->DrawImagePt(bmp, tl);
	}
	else if (this->imgList)
	{
		UOSInt frameNum;
		if (this->imgList->GetCount() <= 1)
		{
			frameNum = 0;
		}
		else
		{
			Double t = clk->GetTimeDiff();
			frameNum = (UInt32)Double2Int32((t - this->startTime) * 1000 / OSInt2Double(this->frameDelay));
			while (frameNum >= this->imgList->GetCount())
			{
				frameNum -= this->imgList->GetCount();
				this->startTime += OSInt2Double(this->frameDelay * (OSInt)this->imgList->GetCount()) * 0.001;
			}
		}
		this->imgList->ToStaticImage(frameNum);
		NotNullPtr<Media::StaticImage> img;
		if (img.Set((Media::StaticImage*)this->imgList->GetImage(frameNum, 0)))
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
