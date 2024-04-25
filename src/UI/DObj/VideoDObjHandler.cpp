#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "Media/DrawEngine.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "UI/GUIForm.h"
#include "UI/DObj/DirectObject.h"
#include "UI/DObj/VideoDObjHandler.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

void __stdcall UI::DObj::VideoDObjHandler::OnTimerTick(AnyType userObj)
{
/*	NN<UI::DObj::VideoDObjHandler> me = userObj.GetNN<UI::DObj::VideoDObjHandler>();
	me->graph->CheckStatus();
	if (me->graph->IsCompleted())
	{
		me->graph->SeekToTime(0);
	}*/
}

void __stdcall UI::DObj::VideoDObjHandler::OnPBEnd(AnyType userObj)
{
	NN<UI::DObj::VideoDObjHandler> me = userObj.GetNN<UI::DObj::VideoDObjHandler>();
	me->player->SeekTo(0);
	me->player->StartPlayback();
}

void UI::DObj::VideoDObjHandler::DrawBkg(NN<Media::DrawImage> dimg)
{
	ImageDObjHandler::DrawBkg(dimg);
	this->DrawVideo(dimg);
}

void UI::DObj::VideoDObjHandler::DrawVideo(NN<Media::DrawImage> dimg)
{
	NN<Media::DrawImage> frameImg;
	if (frameImg.Set(this->frameImg))
	{
		Sync::MutexUsage mutUsage(this->frameMut);
		dimg->DrawImagePt(frameImg, this->videoTL.ToDouble());
	}
}

void UI::DObj::VideoDObjHandler::LockUpdateSize(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->frameMut);
}

void UI::DObj::VideoDObjHandler::DrawFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<OSInt> destTL, Math::Size2D<UOSInt> buffSize, Bool clearScn)
{
#if defined(VERBOSE)
	printf("VideoDObjHandler DrawFromSurface\r\n");
#endif
	if (this->frameImg)
	{
		Sync::MutexUsage mutUsage(this->frameMut);
		Bool revOrder;
		UInt8 *bits = this->frameImg->GetImgBits(revOrder);
		if (bits)
		{
			surface->GetRasterData(bits, 0, 0, buffSize.x, buffSize.y, this->frameImg->GetImgBpl(), revOrder, Media::RotateType::None);
			this->frameImg->GetImgBitsEnd(true);
			this->shown = false;
		}
	}
}

UI::DObj::VideoDObjHandler::VideoDObjHandler(UI::GUIForm *ownerFrm, NN<Media::DrawEngine> deng, NN<Media::ColorManagerSess> colorSess, NN<Media::MonitorSurfaceMgr> surfaceMgr, Parser::ParserList *parsers, Text::CStringNN imageFileName, Math::Coord2D<OSInt> videoTL, Math::Size2D<UOSInt> videoSize, Text::CStringNN videoFileName) : UI::DObj::ImageDObjHandler(deng, imageFileName), Media::VideoRenderer(colorSess.Ptr(), surfaceMgr, 4, 4)
{
#if defined(VERBOSE)
	printf("VideoDObjHandler init: w = %d, h = %d\r\n", (UInt32)videoSize.x, (UInt32)videoSize.y);
#endif
	this->parsers = this->parsers;
	this->videoTL = videoTL;
	this->videoSize = videoSize;
	this->ownerFrm = ownerFrm;
	this->mf = 0;
	this->player = 0;
	this->UpdateDispInfo(videoSize, 32, Media::PF_B8G8R8A8);
	this->videoFileName = Text::String::New(videoFileName);
	this->frameImg = this->deng->CreateImage32(videoSize, Media::AT_NO_ALPHA);
	{
		IO::StmData::FileData fd(videoFileName, false);
		this->mf = (Media::MediaFile*)parsers->ParseFileType(fd, IO::ParserType::MediaFile);
	}
	if (this->mf)
	{
#if defined(VERBOSE)
		printf("VideoDObjHandler Media file parsed\r\n");
#endif
		NEW_CLASS(this->player, Media::MediaPlayer(this, 0));
		this->player->LoadMedia(this->mf);
		this->player->StartPlayback();
		this->player->SetEndHandler(OnPBEnd, this);
	}
	this->tmr = this->ownerFrm->AddTimer(100, OnTimerTick, this);
}

UI::DObj::VideoDObjHandler::~VideoDObjHandler()
{
	if (this->player)
	{
		this->player->StopPlayback();
		DEL_CLASS(this->player);
	}
	this->ownerFrm->RemoveTimer(this->tmr);
	NN<Media::DrawImage> img;
	if (img.Set(this->frameImg))
	{
		this->deng->DeleteImage(img);
	}
	this->videoFileName->Release();
	SDEL_CLASS(this->mf);
}

void UI::DObj::VideoDObjHandler::UpdateVideoArea(Math::Coord2D<OSInt> videoTL, Math::Size2D<UOSInt> videoSize)
{
	Sync::MutexUsage mutUsage(this->frameMut);
	NN<Media::DrawImage> img;
	if (img.Set(this->frameImg))
	{
		this->deng->DeleteImage(img);
	}
	this->videoTL = videoTL;
	this->videoSize = videoSize;
	this->frameImg = this->deng->CreateImage32(videoSize, Media::AT_NO_ALPHA);
	this->UpdateDispInfo(videoSize, 32, Media::PF_B8G8R8A8);
}

void UI::DObj::VideoDObjHandler::SetRotateType(Media::RotateType rotType)
{
}

Media::RotateType UI::DObj::VideoDObjHandler::GetRotateType() const
{
	return Media::RotateType::None;
}
