#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "UI/DObj/OverlayDObj.h"

UI::DObj::OverlayDObj::OverlayDObj(Media::DrawEngine *deng, Media::DrawImage *bmp, OSInt left, OSInt top) : DirectObject(left, top)
{
	this->deng = deng;
	this->noRelease = true;
	this->bmp = bmp;
	this->imgList = 0;
	this->frameDelay = 500;
	this->clk = 0;
}

UI::DObj::OverlayDObj::OverlayDObj(Media::DrawEngine *deng, const UTF8Char *fileName, OSInt left, OSInt top, Parser::ParserList *parsers) : DirectObject(left, top)
{
	this->deng = deng;
	this->noRelease = false;
	this->bmp = 0;
	this->clk = 0;
	if (fileName == 0)
	{
		this->imgList = 0;
	}
	else
	{
		IO::StmData::FileData *fd;
		NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
		this->imgList = (Media::ImageList*)parsers->ParseFileType(fd,IO::ParserType::ImageList);
		DEL_CLASS(fd);
		this->frameDelay = 500;
		this->startTime = 0;
		this->lastFrameNum = -1;
		NEW_CLASS(this->clk, Manage::HiResClock());
	}
}

UI::DObj::OverlayDObj::~OverlayDObj()
{
	if (this->noRelease)
	{
	}
	else if (this->bmp)
	{
		this->deng->DeleteImage(this->bmp);
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
		OSInt i = Math::Double2Int32((t - this->startTime) * 1000 / this->frameDelay);
		while (i >= (OSInt)this->imgList->GetCount())
		{
			i -= this->imgList->GetCount();
			this->startTime += (this->frameDelay * this->imgList->GetCount()) * 0.001;
		}
		return i != this->lastFrameNum;
	}
	return false;
}

Bool UI::DObj::OverlayDObj::DoEvents()
{
	return false;
}

void UI::DObj::OverlayDObj::DrawObject(Media::DrawImage *dimg)
{
	if (this->bmp)
	{
		OSInt left;
		OSInt top;
		GetCurrPos(&left, &top);
		dimg->DrawImagePt(this->bmp, Math::OSInt2Double(left), Math::OSInt2Double(top));
	}
	else if (this->imgList)
	{
		OSInt frameNum;
		if (this->imgList->GetCount() <= 1)
		{
			frameNum = 0;
		}
		else
		{
			Double t = clk->GetTimeDiff();
			frameNum = Math::Double2Int32((t - this->startTime) * 1000 / this->frameDelay);
			while (frameNum >= (OSInt)this->imgList->GetCount())
			{
				frameNum -= this->imgList->GetCount();
				this->startTime += (this->frameDelay * this->imgList->GetCount()) * 0.001;
			}
		}
		this->imgList->ToStaticImage(frameNum);
		Media::StaticImage *img = (Media::StaticImage*)this->imgList->GetImage(frameNum, 0);
		OSInt left;
		OSInt top;
		GetCurrPos(&left, &top);
		if (img)
		{
			dimg->DrawImagePt2(img, Math::OSInt2Double(left), Math::OSInt2Double(top));
		}
		else
		{
			img = 0;
		}
	}
}

Bool UI::DObj::OverlayDObj::IsObject(OSInt x, OSInt y)
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
