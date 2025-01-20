#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "Sync/MutexUsage.h"
#include "UI/DObj/SizedOverlayDObj.h"

UI::DObj::SizedOverlayDObj::SizedOverlayDObj(NN<Media::DrawEngine> deng, Text::CString fileName, Math::Coord2D<OSInt> tl, Math::Size2D<UOSInt> size, NN<Parser::ParserList> parsers, NN<Media::Resizer::LanczosResizer8_C8> resizer) : DirectObject(tl)
{
	this->deng = deng;
	this->noRelease = false;
	this->size = size;
	this->clk = 0;
	this->resizer = resizer;
	this->dispImg = 0;
	this->dispFrameNum = 0;
	this->drawOfst = Math::Coord2DDbl(0, 0);
	if (fileName.leng == 0)
	{
		this->imgList = 0;
	}
	else
	{
		{
			IO::StmData::FileData fd(fileName.OrEmpty(), false);
			this->imgList = Optional<Media::ImageList>::ConvertFrom(parsers->ParseFileType(fd, IO::ParserType::ImageList));
		}
		this->frameDelay = 500;
		NN<Media::ImageList> imgList;
		if (this->imgList.SetTo(imgList))
		{
			UInt32 frameDelay = imgList->GetImageDelay(0);
			if (frameDelay > 0)
			{
				this->frameDelay = (OSInt)frameDelay;
			}
		}
		this->startTime = 0;
		this->lastFrameNum = -1;
		NEW_CLASSOPT(this->clk, Manage::HiResClock());
	}
}

UI::DObj::SizedOverlayDObj::~SizedOverlayDObj()
{
	if (this->noRelease)
	{
	}
	else if (this->imgList.NotNull())
	{
		this->imgList.Delete();
	}
	this->dispImg.Delete();
	this->clk.Delete();
}

Bool UI::DObj::SizedOverlayDObj::IsChanged()
{
	Sync::MutexUsage mutUsage(this->imgMut);
	NN<Media::ImageList> imgList;
	NN<Manage::HiResClock> clk;
	if (this->imgList.SetTo(imgList) && this->clk.SetTo(clk))
	{
		if (imgList->GetCount() <= 1)
			return false;
		if (this->dispImg.IsNull())
			return true;
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

Bool UI::DObj::SizedOverlayDObj::DoEvents()
{
	return false;
}

void UI::DObj::SizedOverlayDObj::DrawObject(NN<Media::DrawImage> dimg)
{
	Sync::MutexUsage imgMutUsage(this->imgMut);
	NN<Media::ImageList> imgList;
	NN<Manage::HiResClock> clk;
	if (this->imgList.SetTo(imgList) && this->clk.SetTo(clk))
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
		Sync::MutexUsage mutUsage(this->dispMut);
		NN<Media::StaticImage> img;
		if (this->dispImg.IsNull() || this->dispFrameNum != frameNum)
		{
			this->dispImg.Delete();
			imgList->ToStaticImage(frameNum);
			this->dispFrameNum = frameNum;
			if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(frameNum, 0)).SetTo(img))
			{
				img->To32bpp();
				this->resizer->SetResizeAspectRatio(Media::ImageResizer::RAR_SQUAREPIXEL);
				this->resizer->SetTargetSize(this->size);
				this->dispImg = this->resizer->ProcessToNew(img);
				if (this->dispImg.SetTo(img))
				{
					this->drawOfst = (this->size - img->info.dispSize).ToDouble() * 0.5;
				}
			}
			else
			{
				this->dispImg = 0;
			}
		}
		if (this->dispImg.SetTo(img))
		{
			Math::Coord2DDbl tl = GetCurrPos().ToDouble();
			dimg->DrawImagePt2(img, tl + this->drawOfst);
		}
	}
}

Bool UI::DObj::SizedOverlayDObj::IsObject(Math::Coord2D<OSInt> scnPos)
{
	return false;
}

void UI::DObj::SizedOverlayDObj::OnMouseDown()
{
}

void UI::DObj::SizedOverlayDObj::OnMouseUp()
{
}

void UI::DObj::SizedOverlayDObj::OnMouseClick()
{
}

void UI::DObj::SizedOverlayDObj::SetFrameDelay(OSInt frameDelay)
{
	if (frameDelay > 0)
	{
		this->frameDelay = frameDelay;
	}
}

void UI::DObj::SizedOverlayDObj::SetSize(Math::Size2D<UOSInt> size)
{
	this->size = size;
	Sync::MutexUsage mutUsage(this->dispMut);
	this->dispImg.Delete();	
}

void UI::DObj::SizedOverlayDObj::SetImage(Text::CStringNN fileName, NN<Parser::ParserList> parsers)
{
	Optional<Media::ImageList> imgList;
	{
		IO::StmData::FileData fd(fileName, false);
		imgList = Optional<Media::ImageList>::ConvertFrom(parsers->ParseFileType(fd, IO::ParserType::ImageList));
	}
	{
		Sync::MutexUsage imgMutUsage(this->imgMut);
		this->imgList.Delete();
		this->imgList = imgList;
	}
}
